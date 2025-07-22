#include "Engine.h"
#include "Application.h"

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

	MessageBox(NULL, "Running app", "Message", MB_OK);

	ApplicationInstance->End();
}
