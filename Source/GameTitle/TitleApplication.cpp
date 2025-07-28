#include "TitleApplication.h"
#include "Core/ConsoleOutput.h"

#include "Core/Delegate.h"

IMPLEMENT(TitleApplication)

Core::Delegate d;

void Free()
{
	CONSOLE_PRINTF("Free print");
}

TitleApplication::TitleApplication(Core::Engine& EngineInstanceRunningApplication)
	: Core::Application(EngineInstanceRunningApplication)
{
	GetEngine().CreateConsoleWindow();
	CONSOLE_PRINTF("hello title application\n");

	d.BindMethod<TitleApplication, &TitleApplication::Print>(this);
	d.BindFunction<&Free>();
	d();
}

void TitleApplication::Print()
{
	CONSOLE_PRINTF("TA Print");
}
