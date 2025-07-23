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

	//while (RunningApplicationInstance)
	//{
	//	Platform::Tick();
	//	ApplicationInstance->Tick();
	//}

	MessageBox(NULL, "App running", "Message", MB_OK);

	ApplicationInstance->End();
}
