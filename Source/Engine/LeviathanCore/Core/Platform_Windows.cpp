#include "Platform.h"

namespace
{
	namespace WindowsPlatformInternals
	{
		LARGE_INTEGER TicksPerSecond = {};
		LARGE_INTEGER LastTickCount = {};
		uint64_t FrameMicroseconds = 0;

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
	}
}

bool Core::Platform::Initialize()
{
	WindowsPlatformInternals::InitializePerformanceCounter();

	return true;
}

void Core::Platform::PerFrameUpdate()
{
	WindowsPlatformInternals::UpdatePerformanceCounter();
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
