#include "Platform.h"
#include "NotificationManager.h"

namespace
{
	namespace WindowsPlatformInternals
	{
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
						GameControllerConnectedNotificationData.Payload.GameControllerConnected = {};

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
						GameControllerDisconnectedNotificationData.Payload.GameControllerDisconnected = {};

						WindowsPlatformInternals::pEngineNotificationManager->SendNotification(GameControllerDisconnectedNotificationData);
					}
				}

				return 0;
			}

			default:
				return DefWindowProc(hWnd, Msg, wParam, lParam);
			}
		}

		bool CreateInvisibleWindow()
		{
			WNDCLASSEX InvisibleWindowClass = {};
			InvisibleWindowClass.cbSize = sizeof(WNDCLASSEX);
			InvisibleWindowClass.style = 0;
			InvisibleWindowClass.lpfnWndProc = &WindowsPlatformInternals::InvisibleWindowWndProc;
			InvisibleWindowClass.cbClsExtra = 0;
			InvisibleWindowClass.cbWndExtra = 0;
			InvisibleWindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			InvisibleWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
			InvisibleWindowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
			InvisibleWindowClass.lpszMenuName = NULL;
			InvisibleWindowClass.lpszClassName = "InvisibleWindowClass";

			if (!RegisterClassEx(&InvisibleWindowClass))
			{
				return false;
			}

			WindowsPlatformInternals::InvisibleWindowHandle = CreateWindowEx(0, InvisibleWindowClass.lpszClassName, "", 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
			if (WindowsPlatformInternals::InvisibleWindowHandle == NULL)
			{
				return false;
			}

			return true;
		}

		// Returns true if the raw input devices were succesfully registered otherwise, returns false. 
		// Requires the invisible window to have been created and the invisible window handle to be valid
		bool RegisterWindowsRawInputDevices()
		{
			if (WindowsPlatformInternals::InvisibleWindowHandle == NULL)
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
			Devices[1].hwndTarget = WindowsPlatformInternals::InvisibleWindowHandle; // Game controller connected/disconnected messages require a specific window to send messages to

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
	}
}

bool Core::Platform::Initialize(Core::NotificationManager* pNotificationManager)
{
	WindowsPlatformInternals::InitializePerformanceCounter();

	WindowsPlatformInternals::pEngineNotificationManager = pNotificationManager;

	// Create an invisible window that is used to listen for gamepad connected/disconnected messages
	if (!WindowsPlatformInternals::CreateInvisibleWindow())
	{
		return false;
	}

	// Register raw input devices
	if (!WindowsPlatformInternals::RegisterWindowsRawInputDevices())
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
