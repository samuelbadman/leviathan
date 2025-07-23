#include "Engine.h"
#include "Application.h"
#include "Platform.h"

void Core::Engine::BeginApplication(std::unique_ptr<Application> pApplication)
{
	if (!pApplication)
	{
		return;
	}

	ApplicationInstance = std::move(pApplication);
	RunningApplicationInstance = true;

	if (!Platform::Initialize())
	{
		return;
	}

	BeginApplicationMainLoop();
}

void Core::Engine::BeginApplicationMainLoop()
{
	ApplicationInstance->Begin();

	while (RunningApplicationInstance)
	{
		Platform::PerFrameUpdate();

		const double FrameDeltaSeconds = Platform::GetFrameMicroseconds() * 1e-6;

		//const double AverageFPS = 1.0 / FrameDeltaSeconds;
		//const double AverageMilliseconds = 1.0 / AverageFPS;
		//const uint32_t AverageFPSWhole = static_cast<uint32_t>(AverageFPS);

		TimeAccumulationSeconds += FrameDeltaSeconds;
		while (TimeAccumulationSeconds > TimeElapsedBetweenFixedTicksSeconds)
		{
			ApplicationInstance->FixedTick(FixedTimestep);
			TimeAccumulationSeconds -= TimeElapsedBetweenFixedTicksSeconds;
		}

		ApplicationInstance->Tick(FrameDeltaSeconds);
	}

	ApplicationInstance->End();
}
