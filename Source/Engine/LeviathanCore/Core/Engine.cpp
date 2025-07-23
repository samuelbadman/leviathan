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
		Platform::Tick();

		const float DeltaSeconds = Platform::Microseconds() * 1e-6f;
		const float AverageFPS = 1.0f / DeltaSeconds;
		const float AverageMilliseconds = 1.0f / AverageFPS;
		const uint16_t AverageFPSWhole = static_cast<uint16_t>(AverageFPS);

		//printf("fps: %d, ms: %f\n", AverageFPSWhole, AverageMilliseconds);

		ApplicationInstance->Tick(DeltaSeconds);
	}

	MessageBox(NULL, "App running", "Message", MB_OK);

	ApplicationInstance->End();
}
