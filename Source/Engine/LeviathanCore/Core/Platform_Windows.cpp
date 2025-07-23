#include "Platform.h"

namespace
{
	namespace WinPlatformInternals
	{
		LARGE_INTEGER TicksPerSecond = {};
		LARGE_INTEGER LastTickCount = {};
		uint64_t Microseconds  = 0;

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
	}
}

bool Core::Platform::Initialize()
{
	WinPlatformInternals::InitializePerformanceCounter();


	return true;
}

void Core::Platform::Tick()
{
	WinPlatformInternals::UpdatePerformanceCounter();
}
