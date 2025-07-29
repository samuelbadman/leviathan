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
	CONSOLE_PRINTF("hello title application\n\n");

	SD.BindFunction(&Free);
	SD.BindMethod<TitleApplication, &TitleApplication::Print>(this);
	SD.BindLambda([]() { CONSOLE_PRINTF("lambda print\n"); });

	MD.AddFunction(&Free);
	MD.AddFunction(&Free);
	MD.AddMethod<TitleApplication, &TitleApplication::Print>(this);
	MD.AddMethod<TitleApplication, &TitleApplication::Print>(this);
	MD.AddLambda([]() { CONSOLE_PRINTF("lambda print\n"); });
	MD.AddLambda([]() { CONSOLE_PRINTF("lambda print\n"); });
}

void TitleApplication::Print()
{
	CONSOLE_PRINTF("title app method print\n");
}

void TitleApplication::Begin()
{
	SD.Execute();

	CONSOLE_PRINTF("\n\n");

	MD.Execute();
}

