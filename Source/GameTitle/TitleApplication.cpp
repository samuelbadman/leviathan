#include "TitleApplication.h"
#include "Core/ConsoleOutput.h"

IMPLEMENT(TitleApplication)

TitleApplication::TitleApplication(Core::Engine& EngineInstanceRunningApplication)
	: Core::Application(EngineInstanceRunningApplication)
{
	GetEngine().CreateConsoleWindow();
	CONSOLE_PRINTF("hello title application\n");
}