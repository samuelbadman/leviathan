#include "Engine.h"
#include "Application.h"

#include <cstdio>

void Core::Engine::BeginApplication(std::unique_ptr<Application> pApplication)
{
	if (!pApplication)
	{
		return;
	}

	ApplicationInstance = std::move(pApplication);
	RunningApplicationInstance = true;

	BeginApplicationMainLoop();
}

void Core::Engine::BeginApplicationMainLoop()
{
	ApplicationInstance->Begin();

	//while (RunningApplicationInstance)
	//{
	//	ApplicationInstance->Tick();
	//}

#ifdef PLATFORM_WINDOWS
	MessageBox(NULL, "Running app", "Message", MB_OK);
#else
	printf("Running app");
#endif // PLATFORM_WINDOWS

	ApplicationInstance->End();
}
