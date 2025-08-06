#include "Platform.h"
#include "Core/NotificationManager.h"
#include "Core/Window.h"

namespace
{
	namespace WindowsPlatformInternals
	{
		constexpr DWORD WindowStyle_Windowed = WS_OVERLAPPEDWINDOW;
		constexpr DWORD WindowStyle_Borderless = WS_POPUPWINDOW;
		constexpr DWORD WindowStyle_NoResize = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		constexpr DWORD WindowStyle_NoDragSize = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME;

		Core::NotificationManager* pEngineNotificationManager = nullptr;

		LARGE_INTEGER TicksPerSecond = {};
		LARGE_INTEGER LastTickCount = {};
		uint64_t FrameMicroseconds = 0;

		HWND InvisibleWindowHandle = NULL;

		void InitializePerformanceCounter()
		{
			QueryPerformanceFrequency(&TicksPerSecond);
			QueryPerformanceCounter(&LastTickCount);
		}

		void UpdatePerformanceCounter()
		{
			LARGE_INTEGER CurrentTickCount;
			QueryPerformanceCounter(&CurrentTickCount);

			const uint64_t ElapsedTicks = CurrentTickCount.QuadPart - WindowsPlatformInternals::LastTickCount.QuadPart;
			// Convert elapsed ticks to microseconds to not lose precision by dividing a small number by a large one.
			WindowsPlatformInternals::FrameMicroseconds = (ElapsedTicks * static_cast<uint64_t>(1e6)) / WindowsPlatformInternals::TicksPerSecond.QuadPart;

			WindowsPlatformInternals::LastTickCount = CurrentTickCount;
		}

		// Returns false if the console could not be created otherwise, returns true
		bool CreateConsole()
		{
			if (!AllocConsole())
			{
				return false;
			}

			// Redirect standard IO.
			FILE* file;
			if (!(freopen_s(&file, "CONOUT$", "w", stdout) != 0))
			{
				setvbuf(stdout, nullptr, _IONBF, 0);
			}

			if (!(freopen_s(&file, "CONIN$", "r", stdin) != 0))
			{
				setvbuf(stdin, nullptr, _IONBF, 0);
			}

			if (!(freopen_s(&file, "CONOUT$", "w", stderr) != 0))
			{
				setvbuf(stderr, nullptr, _IONBF, 0);
			}

			std::ios::sync_with_stdio(true);
			std::wcout.clear();
			std::cout.clear();
			std::wcerr.clear();
			std::cerr.clear();
			std::wcin.clear();
			std::cin.clear();

			// Console creation succeeded.
			return true;
		}

		// Returns false if the console could not be freed otherwise, returns true
		bool RemoveConsole()
		{
			// Detach from console.
			if (!FreeConsole())
			{
				// FreeConsole failed.
				return false;
			}

			// Redirect standard IO.
			FILE* file;
			if (!(freopen_s(&file, "NUL:", "r", stdin) != 0))
			{
				setvbuf(stdin, nullptr, _IONBF, 0);
			}

			if (!(freopen_s(&file, "NUL:", "w", stdout) != 0))
			{
				setvbuf(stdout, nullptr, _IONBF, 0);
			}

			if (!(freopen_s(&file, "NUL:", "w", stderr) != 0))
			{
				setvbuf(stderr, nullptr, _IONBF, 0);
			}

			std::ios::sync_with_stdio(false);

			// Console destruction succeeded.
			return true;
		}

		// Window procedure function to handle system messages received by the invisible window created during platform initialization
		LRESULT CALLBACK InvisibleWindowWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
		{
			switch (Msg)
			{
			case WM_INPUT_DEVICE_CHANGE:
			{
				// WM_INPUT_DEVICE_CHANGE message is only received by the window passed to RegisterRawInputDevices
				if (wParam == GIDC_ARRIVAL)
				{
					// Game controller connected.
					if (WindowsPlatformInternals::pEngineNotificationManager)
					{
						Core::NotificationData GameControllerConnectedNotificationData = {};
						GameControllerConnectedNotificationData.Type = Core::NotificationType::GameControllerConnected;
						GameControllerConnectedNotificationData.Payload.GameControllerConnectedPayload = {};

						WindowsPlatformInternals::pEngineNotificationManager->SendNotification(GameControllerConnectedNotificationData);
					}
				}
				else if (wParam == GIDC_REMOVAL)
				{
					// Game controller disconnected.
					if (WindowsPlatformInternals::pEngineNotificationManager)
					{
						Core::NotificationData GameControllerDisconnectedNotificationData = {};
						GameControllerDisconnectedNotificationData.Type = Core::NotificationType::GameControllerDisconnected;
						GameControllerDisconnectedNotificationData.Payload.GameControllerDisconnectedPayload = {};

						WindowsPlatformInternals::pEngineNotificationManager->SendNotification(GameControllerDisconnectedNotificationData);
					}
				}

				return 0;
			}

			default:
				return DefWindowProc(hWnd, Msg, wParam, lParam);
			}
		}

		// Creates a new Win32 window with the specified parameters. ClassName parameter must be unique from all windows created. Returns true if the window
		// is succesfully created otherwise, returns false. The HWND handle to the created window is returned in OutWindowHandle
		bool CreateWindowAndReturnHandle(HWND& OutWindowHandle, WNDPROC WndProc, LPCSTR ClassName, UINT ClassStyle, LPCSTR WindowName, DWORD Style, int32_t X, int32_t Y,
			int32_t Width, int32_t Height, HWND hWndParent, LPVOID CreateParameters)
		{
			WNDCLASSEX WindowClass = {};
			WindowClass.cbSize = sizeof(WNDCLASSEX);
			WindowClass.style = ClassStyle;
			WindowClass.lpfnWndProc = WndProc;
			WindowClass.cbClsExtra = 0;
			WindowClass.cbWndExtra = 0;
			WindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
			WindowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
			WindowClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
			WindowClass.lpszMenuName = NULL;
			WindowClass.lpszClassName = ClassName;

			if (!RegisterClassEx(&WindowClass))
			{
				return false;
			}

			OutWindowHandle = CreateWindowEx(0, WindowClass.lpszClassName, WindowName, Style, X, Y, Width, Height,
				hWndParent, NULL, NULL, CreateParameters);

			if (OutWindowHandle == NULL)
			{
				return false;
			}

			return true;
		}

		// Returns true if the raw input devices were succesfully registered otherwise, returns false. 
		// Requires the invisible window to have been created and the invisible window handle to be valid
		bool RegisterWindowsRawInputDevices(HWND HandleToWindowReceivingDeviceConnectionMessages)
		{
			if (HandleToWindowReceivingDeviceConnectionMessages == NULL)
			{
				return false;
			}

			static constexpr USHORT RawInputDeviceMouseUsagePage = 0x01;
			static constexpr USHORT RawInputDeviceMouseUsage = 0x02;
			static constexpr USHORT RawInputDeviceGameControllerUsagePage = 0x01;
			static constexpr USHORT RawInputDeviceGameControllerUsage = 0x05;
			// Generate WM_INPUT_DEVICE_CHANGED messages in the WndProc function when this device is added/removed
			static constexpr DWORD RawInputDeviceGameControllerFlags = RIDEV_DEVNOTIFY;

			std::array<RAWINPUTDEVICE, 2> Devices = {};

			Devices[0].usUsagePage = RawInputDeviceMouseUsagePage;
			Devices[0].usUsage = RawInputDeviceMouseUsage;
			Devices[0].dwFlags = 0;
			Devices[0].hwndTarget = NULL; // Null hwnd to have generated raw input messages be sent to the window that currently has keyboard focus

			Devices[1].usUsagePage = RawInputDeviceGameControllerUsagePage;
			Devices[1].usUsage = RawInputDeviceGameControllerUsage;
			Devices[1].dwFlags = RawInputDeviceGameControllerFlags;
			Devices[1].hwndTarget = HandleToWindowReceivingDeviceConnectionMessages; // Game controller connected/disconnected messages require a specific window to send messages to

			return (RegisterRawInputDevices(Devices.data(), Devices.size(), sizeof(RAWINPUTDEVICE)) == TRUE);
		}

		void DispatchSystemMessages()
		{
			MSG Msg;
			while (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
		}

		Core::InputKey TranslateVirtualKey(WORD VirtualKey, WORD ScanCode)
		{
			switch (VirtualKey)
			{
				/*Backspace*/
			case VK_BACK: return Core::InputKey(Core::Keys::Backspace, ScanCode);
				/*Tab*/
			case VK_TAB: return Core::InputKey(Core::Keys::Tab, ScanCode);
				/*Enter*/
			case VK_RETURN: return Core::InputKey(Core::Keys::Enter, ScanCode);
				/*CapsLock*/
			case VK_CAPITAL: return Core::InputKey(Core::Keys::CapsLock, ScanCode);
				/*Escape*/
			case VK_ESCAPE: return Core::InputKey(Core::Keys::Escape, ScanCode);
				/*SpaceBar*/
			case VK_SPACE: return Core::InputKey(Core::Keys::SpaceBar, ScanCode);
				/*PageUp*/
			case VK_PRIOR: return Core::InputKey(Core::Keys::PageUp, ScanCode);
				/*PageDown*/
			case VK_NEXT: return Core::InputKey(Core::Keys::PageDown, ScanCode);
				/*End*/
			case VK_END: return Core::InputKey(Core::Keys::End, ScanCode);
				/*Home*/
			case VK_HOME: return Core::InputKey(Core::Keys::Home, ScanCode);
				/*Insert*/
			case VK_INSERT: return Core::InputKey(Core::Keys::Insert, ScanCode);
				/*Delete*/
			case VK_DELETE: return Core::InputKey(Core::Keys::Delete, ScanCode);
				/*Left*/
			case VK_LEFT: return Core::InputKey(Core::Keys::Left, ScanCode);
				/*Right*/
			case VK_RIGHT: return Core::InputKey(Core::Keys::Right, ScanCode);
				/*Up*/
			case VK_UP: return Core::InputKey(Core::Keys::Up, ScanCode);
				/*Down*/
			case VK_DOWN: return Core::InputKey(Core::Keys::Down, ScanCode);

				/*Zero*/
			case '0': return Core::InputKey(Core::Keys::Zero, ScanCode);
				/*One*/
			case '1': return Core::InputKey(Core::Keys::One, ScanCode);
				/*Two*/
			case '2': return Core::InputKey(Core::Keys::Two, ScanCode);
				/*Three*/
			case '3': return Core::InputKey(Core::Keys::Three, ScanCode);
				/*Four*/
			case '4': return Core::InputKey(Core::Keys::Four, ScanCode);
				/*Five*/
			case '5': return Core::InputKey(Core::Keys::Five, ScanCode);
				/*Six*/
			case '6': return Core::InputKey(Core::Keys::Six, ScanCode);
				/*Seven*/
			case '7': return Core::InputKey(Core::Keys::Seven, ScanCode);
				/*Eight*/
			case '8': return Core::InputKey(Core::Keys::Eight, ScanCode);
				/*Nine*/
			case '9': return Core::InputKey(Core::Keys::Nine, ScanCode);
				/*A*/
			case 'A': return Core::InputKey(Core::Keys::A, ScanCode);
				/*B*/
			case 'B': return Core::InputKey(Core::Keys::B, ScanCode);
				/*C*/
			case 'C': return Core::InputKey(Core::Keys::C, ScanCode);
				/*D*/
			case 'D': return Core::InputKey(Core::Keys::D, ScanCode);
				/*E*/
			case 'E': return Core::InputKey(Core::Keys::E, ScanCode);
				/*F*/
			case 'F': return Core::InputKey(Core::Keys::F, ScanCode);
				/*G*/
			case 'G': return Core::InputKey(Core::Keys::G, ScanCode);
				/*H*/
			case 'H': return Core::InputKey(Core::Keys::H, ScanCode);
				/*I*/
			case 'I': return Core::InputKey(Core::Keys::I, ScanCode);
				/*J*/
			case 'J': return Core::InputKey(Core::Keys::J, ScanCode);
				/*K*/
			case 'K': return Core::InputKey(Core::Keys::K, ScanCode);
				/*L*/
			case 'L': return Core::InputKey(Core::Keys::L, ScanCode);
				/*M*/
			case 'M': return Core::InputKey(Core::Keys::M, ScanCode);
				/*N*/
			case 'N': return Core::InputKey(Core::Keys::N, ScanCode);
				/*O*/
			case 'O': return Core::InputKey(Core::Keys::O, ScanCode);
				/*P*/
			case 'P': return Core::InputKey(Core::Keys::P, ScanCode);
				/*Q*/
			case 'Q': return Core::InputKey(Core::Keys::Q, ScanCode);
				/*R*/
			case 'R': return Core::InputKey(Core::Keys::R, ScanCode);
				/*S*/
			case 'S': return Core::InputKey(Core::Keys::S, ScanCode);
				/*T*/
			case 'T': return Core::InputKey(Core::Keys::T, ScanCode);
				/*U*/
			case 'U': return Core::InputKey(Core::Keys::U, ScanCode);
				/*V*/
			case 'V': return Core::InputKey(Core::Keys::V, ScanCode);
				/*W*/
			case 'W': return Core::InputKey(Core::Keys::W, ScanCode);
				/*X*/
			case 'X': return Core::InputKey(Core::Keys::X, ScanCode);
				/*Y*/
			case 'Y': return Core::InputKey(Core::Keys::Y, ScanCode);
				/*Z*/
			case 'Z': return Core::InputKey(Core::Keys::Z, ScanCode);
				/*Numpad0*/
			case VK_NUMPAD0: return Core::InputKey(Core::Keys::Numpad0, ScanCode);
				/*Numpad1*/
			case VK_NUMPAD1: return Core::InputKey(Core::Keys::Numpad1, ScanCode);
				/*Numpad2*/
			case VK_NUMPAD2: return Core::InputKey(Core::Keys::Numpad2, ScanCode);
				/*Numpad3*/
			case VK_NUMPAD3: return Core::InputKey(Core::Keys::Numpad3, ScanCode);
				/*Numpad4*/
			case VK_NUMPAD4: return Core::InputKey(Core::Keys::Numpad4, ScanCode);
				/*Numpad5*/
			case VK_NUMPAD5: return Core::InputKey(Core::Keys::Numpad5, ScanCode);
				/*Numpad6*/
			case VK_NUMPAD6: return Core::InputKey(Core::Keys::Numpad6, ScanCode);
				/*Numpad7*/
			case VK_NUMPAD7: return Core::InputKey(Core::Keys::Numpad7, ScanCode);
				/*Numpad8*/
			case VK_NUMPAD8: return Core::InputKey(Core::Keys::Numpad8, ScanCode);
				/*Numpad9*/
			case VK_NUMPAD9: return Core::InputKey(Core::Keys::Numpad9, ScanCode);
				/*Numpad**/
			case VK_MULTIPLY: return Core::InputKey(Core::Keys::NumpadMultiply, ScanCode);
				/*Numpad+*/
			case VK_ADD: return Core::InputKey(Core::Keys::NumpadPlus, ScanCode);
				/*Numpad-*/
			case VK_SUBTRACT: return Core::InputKey(Core::Keys::NumpadMinus, ScanCode);
				/*Numpad.*/
			case VK_DECIMAL: return Core::InputKey(Core::Keys::NumpadDecimal, ScanCode);
				/*Numpad/*/
			case VK_DIVIDE: return Core::InputKey(Core::Keys::NumpadDivide, ScanCode);
				/*F1*/
			case VK_F1: return Core::InputKey(Core::Keys::F1, ScanCode);
				/*F2*/
			case VK_F2: return Core::InputKey(Core::Keys::F2, ScanCode);
				/*F3*/
			case VK_F3: return Core::InputKey(Core::Keys::F3, ScanCode);
				/*F4*/
			case VK_F4: return Core::InputKey(Core::Keys::F4, ScanCode);
				/*F5*/
			case VK_F5: return Core::InputKey(Core::Keys::F5, ScanCode);
				/*F6*/
			case VK_F6: return Core::InputKey(Core::Keys::F6, ScanCode);
				/*F7*/
			case VK_F7: return Core::InputKey(Core::Keys::F7, ScanCode);
				/*F8*/
			case VK_F8: return Core::InputKey(Core::Keys::F8, ScanCode);
				/*F9*/
			case VK_F9: return Core::InputKey(Core::Keys::F9, ScanCode);
				/*F10*/
			case VK_F10: return Core::InputKey(Core::Keys::F10, ScanCode);
				/*F11*/
			case VK_F11: return Core::InputKey(Core::Keys::F11, ScanCode);
				/*F12*/
			case VK_F12: return Core::InputKey(Core::Keys::F12, ScanCode);

				/*NumLock*/
			case VK_NUMLOCK: return Core::InputKey(Core::Keys::NumLock, ScanCode);
				/*ScrollLock*/
			case VK_SCROLL: return Core::InputKey(Core::Keys::ScrollLock, ScanCode);

				/*LeftShift*/
			case VK_LSHIFT: return Core::InputKey(Core::Keys::LeftShift, ScanCode);
				/*RightShift*/
			case VK_RSHIFT: return Core::InputKey(Core::Keys::RightShift, ScanCode);
				/*LeftCtrl*/
			case VK_LCONTROL: return Core::InputKey(Core::Keys::LeftCtrl, ScanCode);
				/*RightCtrl*/
			case VK_RCONTROL: return Core::InputKey(Core::Keys::RightCtrl, ScanCode);
				/*LeftAlt*/
			case VK_LMENU: return Core::InputKey(Core::Keys::LeftAlt, ScanCode);
				/*RightAlt*/
			case VK_RMENU: return Core::InputKey(Core::Keys::RightAlt, ScanCode);

				/*SemiColon*/
			case VK_OEM_1: return Core::InputKey(Core::Keys::SemiColon, ScanCode);
				/*EqualsPlus*/
			case VK_OEM_PLUS: return Core::InputKey(Core::Keys::Equals, ScanCode);
				/*CommaLeftAngularBrace*/
			case VK_OEM_COMMA: return Core::InputKey(Core::Keys::Comma, ScanCode);
				/*DashUnderscore*/
			case VK_OEM_MINUS: return Core::InputKey(Core::Keys::Dash, ScanCode);
				/*PeriodRightAngularBrace*/
			case VK_OEM_PERIOD: return Core::InputKey(Core::Keys::Period, ScanCode);
				/*ForwardSlashQuestionMark*/
			case VK_OEM_2: return Core::InputKey(Core::Keys::ForwardSlash, ScanCode);
				/*Apostrophe@*/
			case VK_OEM_3: return Core::InputKey(Core::Keys::Apostrophe, ScanCode);
				/*LeftSquareBraceCurlyBrace*/
			case VK_OEM_4: return Core::InputKey(Core::Keys::LeftBrace, ScanCode);
				/*BackslashPipe*/
			case VK_OEM_5: return Core::InputKey(Core::Keys::Backslash, ScanCode);
				/*RightSquareBraceCurlyBrace*/
			case VK_OEM_6: return Core::InputKey(Core::Keys::RightBrace, ScanCode);
				/*GraveAccentTilde*/
			case VK_OEM_7: return Core::InputKey(Core::Keys::Hash, ScanCode);

				/*Unhandled virtual key*/
			default: return Core::InputKey(Core::Keys::Unknown, ScanCode);
			}
		}

		LRESULT CALLBACK WindowWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
		{
			// Get window instance receiving the message from user data parameter
			Core::Window* const WindowInstance = reinterpret_cast<Core::Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

			if (!WindowInstance)
			{
				return DefWindowProc(hWnd, Msg, wParam, lParam);
			}

			switch (Msg)
			{
				// Mouse input events


				// Keyboard input events
			case WM_SYSKEYUP:
			case WM_KEYUP:
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
			{
				WORD VirtualKeyCode = LOWORD(wParam);

				WORD KeyFlags = HIWORD(lParam);

				WORD ScanCode = LOBYTE(KeyFlags);
				BOOL IsExtendedKey = (KeyFlags & KF_EXTENDED) == KF_EXTENDED;

				if (IsExtendedKey)
				{
					ScanCode = MAKEWORD(ScanCode, 0xE0);
				}

				BOOL WasKeyDown = (KeyFlags & KF_REPEAT) == KF_REPEAT;
				//WORD RepeatCount = LOWORD(lParam);

				BOOL IsKeyReleased = (KeyFlags & KF_UP) == KF_UP;

				switch (VirtualKeyCode)
				{
				case VK_SHIFT:
				case VK_CONTROL:
				case VK_MENU:
					VirtualKeyCode = LOWORD(MapVirtualKeyW(ScanCode, MAPVK_VSC_TO_VK_EX));
					break;
				}

				// Check if the window should be force closed on alt+f4 keyboard shortcut
				bool IsAltF4 = (((lParam & (1 << 29)) != 0) && (static_cast<int16_t>(wParam) == VK_F4));
				if (!IsKeyReleased && IsAltF4)
				{
					WindowInstance->OnForceClose();
					return 0;
				}

				// Translate virtual key code to engine key
				Core::WindowInputEventArgs EventArgs;
				EventArgs.Key = TranslateVirtualKey(VirtualKeyCode, ScanCode);

				if (IsKeyReleased)
				{
					EventArgs.Event = Core::WindowInputEvent::Released;
					EventArgs.Data = 0.0f;
				}
				else
				{
					EventArgs.Event = (WasKeyDown) ? Core::WindowInputEvent::Repeat : Core::WindowInputEvent::Pressed;
					EventArgs.Data = 1.0f;
				}

				WindowInstance->OnInputEvent(EventArgs);
				return 0;
			}

			// Window size events
			case WM_SIZE:
				switch (wParam)
				{
				case SIZE_MAXIMIZED:
					WindowInstance->OnMaximized();
					// Also generate a resized event as the window size has changed but windows does not send a SIZE_RESTORED messaged along with a SIZE_MAXIMIZED
					WindowInstance->OnResized(LOWORD(lParam), HIWORD(lParam));
					return 0;

				case SIZE_MINIMIZED:
					WindowInstance->OnMinimized();
					return 0;

				case SIZE_RESTORED:
					WindowInstance->OnResized(LOWORD(lParam), HIWORD(lParam));
					return 0;

				default: return 0;
				}

				// Window size move events
			case WM_ENTERSIZEMOVE:
				WindowInstance->OnEnterSizeMove();
				return 0;

			case WM_EXITSIZEMOVE:
				WindowInstance->OnExitSizeMove();
				return 0;

				// Window focus events
			case WM_ACTIVATEAPP:
				if (wParam == TRUE)
				{
					WindowInstance->OnReceivedFocus();
				}
				else if (wParam == FALSE)
				{
					WindowInstance->OnLostFocus();
				}

				return 0;

				// Window close events
			case WM_CLOSE:
				WindowInstance->OnCloseSignal();
				return 0;

				// Window destroyed events
			case WM_DESTROY:
				WindowInstance->OnDestroyed();
				return 0;

			default: return DefWindowProc(hWnd, Msg, wParam, lParam);
			}
		}

		LRESULT CALLBACK WindowInitWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
		{
			switch (Msg)
			{
			case WM_NCCREATE:
			{
				// Get the parameters passed to CreateWindow()
				const CREATESTRUCTW* const CreateParameters = reinterpret_cast<CREATESTRUCTW*>(lParam);

				// Assumes the window instance pointer was passed to CreateWindow(). This functions is intended to be used with a window created during CreatePlatformWindow() so
				// this should always be the case and it is safe to get the window instance pointer from the create parameters
				Core::Window* WindowInstance = reinterpret_cast<Core::Window*>(CreateParameters->lpCreateParams);

				// Set user data as the window instance to be retrieved during the window procedure function for created windows
				SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(WindowInstance));

				// Set the window procedure function for the window to use after creation to the window window procedure function
				SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowsPlatformInternals::WindowWndProc));

				// Call the window window procedure function
				return WindowsPlatformInternals::WindowWndProc(hWnd, Msg, wParam, lParam);
			}

			default: return DefWindowProc(hWnd, Msg, wParam, lParam);
			}
		}

		constexpr DWORD TranslateWindowMode(const Core::WindowMode Mode)
		{
			switch (Mode)
			{
			case Core::WindowMode::Windowed: return WindowsPlatformInternals::WindowStyle_Windowed;
			case Core::WindowMode::Borderless: return WindowsPlatformInternals::WindowStyle_Borderless;
			case Core::WindowMode::NoResize: return WindowsPlatformInternals::WindowStyle_NoResize;
			case Core::WindowMode::NoDragSize: return WindowsPlatformInternals::WindowStyle_NoDragSize;
			default: return 0;
			}
		}
	}
}

bool Core::Platform::Initialize(Core::NotificationManager* pNotificationManager)
{
	WindowsPlatformInternals::InitializePerformanceCounter();

	WindowsPlatformInternals::pEngineNotificationManager = pNotificationManager;

	// Create an invisible window that is used to listen for gamepad connected/disconnected messages
	if (!WindowsPlatformInternals::CreateWindowAndReturnHandle(WindowsPlatformInternals::InvisibleWindowHandle, &WindowsPlatformInternals::InvisibleWindowWndProc,
		"InvisibleWindowClass", 0, "", 0, 0, 0, 0, 0, NULL, NULL))
	{
		return false;
	}

	// Register raw input devices
	if (!WindowsPlatformInternals::RegisterWindowsRawInputDevices(WindowsPlatformInternals::InvisibleWindowHandle))
	{
		return false;
	}

	return true;
}

void Core::Platform::PerFrameUpdate()
{
	WindowsPlatformInternals::UpdatePerformanceCounter();
	WindowsPlatformInternals::DispatchSystemMessages();
}

double Core::Platform::GetFrameMicroseconds()
{
	return static_cast<double>(WindowsPlatformInternals::FrameMicroseconds);
}

bool Core::Platform::CreateConsole()
{
	// Disable close button in console window menu bar. Can still close the console window using alt+f4 keys
	//HWND ConsoleWindowHandle = GetConsoleWindow();
	//if (ConsoleWindowHandle != NULL)
	//{
	//	HMENU ConsoleWindowMenuHandle = GetSystemMenu(ConsoleWindowHandle, FALSE);
	//	if (ConsoleWindowMenuHandle != NULL)
	//	{
	//		DeleteMenu(ConsoleWindowMenuHandle, SC_CLOSE, MF_BYCOMMAND);
	//	}
	//}

	return WindowsPlatformInternals::CreateConsole();
}

bool Core::Platform::RemoveConsole()
{
	return WindowsPlatformInternals::RemoveConsole();
}

bool Core::Platform::CreatePlatformWindow(Core::Window& Temp, const Core::WindowCreateParameters& Parameters)
{
	// Try to create the window
	HWND Handle;
	if (!WindowsPlatformInternals::CreateWindowAndReturnHandle(
		Handle,
		&WindowsPlatformInternals::WindowInitWndProc,
		Parameters.UniqueWindowName,
		0,
		Parameters.WindowName,
		WindowsPlatformInternals::TranslateWindowMode(Parameters.Mode),
		Parameters.HorizontalPosition,
		Parameters.VerticalPosition,
		Parameters.Width,
		Parameters.Height,
		(Parameters.ParentWindow) ? static_cast<HWND>(Parameters.ParentWindow->GetPlatformHandle()) : NULL,
		&Temp)
		)
	{
		return false;
	}

	// Setup new window after succesful creation
	Temp.SetPlatformHandle(static_cast<void*>(Handle));

	// Show the window
	ShowWindow(Handle, SW_SHOW);

	return true;
}

bool Core::Platform::DestroyPlatformWindow(Core::Window& WindowToDestroy)
{
	if (!DestroyWindow(static_cast<HWND>(WindowToDestroy.GetPlatformHandle())))
	{
		return false;
	}

	return UnregisterClass(WindowToDestroy.GetUniqueName(), NULL);
}
