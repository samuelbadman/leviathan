#include "Platform.h"

namespace
{
	namespace WinPlatformInternals
	{
		LARGE_INTEGER TicksPerSecond = {};
		LARGE_INTEGER LastTickCount = {};
		uint64_t Microseconds = 0;

		void InitializePerformanceCounter()
		{
			QueryPerformanceFrequency(&TicksPerSecond);
			QueryPerformanceCounter(&LastTickCount);
		}

		void UpdatePerformanceCounter()
		{
			LARGE_INTEGER CurrentTickCount;
			QueryPerformanceCounter(&CurrentTickCount);

			const uint64_t ElapsedTicks = CurrentTickCount.QuadPart - WinPlatformInternals::LastTickCount.QuadPart;
			// Convert elapsed ticks to microseconds to not lose precision by dividing a small number by a large one.
			WinPlatformInternals::Microseconds = (ElapsedTicks * static_cast<uint64_t>(1e6)) / WinPlatformInternals::TicksPerSecond.QuadPart;

			WinPlatformInternals::LastTickCount = CurrentTickCount;
		}

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
	WinPlatformInternals::InitializePerformanceCounter();

#ifdef CONFIG_DEBUG
	WinPlatformInternals::CreateConsole();
#endif // CONFIG_DEBUG

	return true;
}

void Core::Platform::Tick()
{
	WinPlatformInternals::UpdatePerformanceCounter();
}

float Core::Platform::Microseconds()
{
	return static_cast<float>(WinPlatformInternals::Microseconds);
}
