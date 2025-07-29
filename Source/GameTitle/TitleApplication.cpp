#include "TitleApplication.h"
#include "Core/ConsoleOutput.h"

IMPLEMENT(TitleApplication)

void Free()
{
	CONSOLE_PRINTF("free print\n");
}

TitleApplication::TitleApplication(Core::Engine& EngineInstanceRunningApplication)
	: Core::Application(EngineInstanceRunningApplication)
{
	GetEngine().CreateConsoleWindow();
	CONSOLE_PRINTF("hello title application\n");

	D.BindFunction(&Free);
	D.BindMethod<TitleApplication, &TitleApplication::Print>(this);
	D.BindLambda([]() { CONSOLE_PRINTF("lambda print\n"); });
}

void TitleApplication::Print()
{
	CONSOLE_PRINTF("title app method print\n");
}

void TitleApplication::Begin()
{
	D.Execute();
}

