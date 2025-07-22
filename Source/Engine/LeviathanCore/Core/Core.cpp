#include "Core.h"
#include "Application.h"

Core::Application* gApplication = nullptr;

void Core::BeginApplication(Application* pApplication)
{
	if (!pApplication)
	{
		return;
	}

	gApplication = pApplication;



	delete gApplication;
}
