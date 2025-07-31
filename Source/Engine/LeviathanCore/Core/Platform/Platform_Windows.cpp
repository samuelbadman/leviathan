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
			case WM_CLOSE:
				WindowInstance->OnCloseSignal();
				return 0;

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
