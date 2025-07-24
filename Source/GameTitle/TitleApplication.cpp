#include "TitleApplication.h"
#include "Core/ConsoleLogging.h"

IMPLEMENT(TitleApplication)

TitleApplication::TitleApplication(Core::Engine& EngineInstanceRunningApplication)
	: Core::Application(EngineInstanceRunningApplication)
{
	GetEngine().CreateConsoleWindow();
	PRINT("hello title application\n");
}
