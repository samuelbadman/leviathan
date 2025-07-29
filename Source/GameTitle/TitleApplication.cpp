#include "TitleApplication.h"
#include "Core/ConsoleOutput.h"

IMPLEMENT(TitleApplication)

void Free()
{
	CONSOLE_PRINTF("free print\n");
}

void FreeP(int32_t N)
{
	CONSOLE_PRINTF("free print %d\n", N);
}

TitleApplication::TitleApplication(Core::Engine& EngineInstanceRunningApplication)
	: Core::Application(EngineInstanceRunningApplication)
{
	GetEngine().CreateConsoleWindow();
	CONSOLE_PRINTF("hello title application\n\n");

	SD.BindFunction(&Free);
	SD.BindMethod<TitleApplication, &TitleApplication::Print>(this);
	SD.BindLambda([]() { CONSOLE_PRINTF("lambda print\n"); });

	SDP.BindFunction(&FreeP);
	SDP.BindMethod<TitleApplication, &TitleApplication::PrintP>(this);
	SDP.BindLambda([](int32_t N) { CONSOLE_PRINTF("lambda print %d\n", N); });

	MD.AddFunction(&Free);
	MD.AddFunction(&Free);
	MD.AddMethod<TitleApplication, &TitleApplication::Print>(this);
	MD.AddMethod<TitleApplication, &TitleApplication::Print>(this);
	MD.AddLambda([]() { CONSOLE_PRINTF("lambda print\n"); });
	MD.AddLambda([]() { CONSOLE_PRINTF("lambda print\n"); });

	MDP.AddFunction(&FreeP);
	MDP.AddFunction(&FreeP);
	MDP.AddMethod<TitleApplication, &TitleApplication::PrintP>(this);
	MDP.AddMethod<TitleApplication, &TitleApplication::PrintP>(this);
	MDP.AddLambda([](int32_t N) { CONSOLE_PRINTF("lambda print %d\n", N); });
	MDP.AddLambda([](int32_t N) { CONSOLE_PRINTF("lambda print %d\n", N); });
}

void TitleApplication::Print()
{
	CONSOLE_PRINTF("title app method print\n");
}

void TitleApplication::PrintP(int32_t N)
{
	CONSOLE_PRINTF("title app method print %d\n", N);
}

void TitleApplication::Begin()
{
	SD.Execute();

	CONSOLE_PRINTF("\n\n");

	MD.Execute();

	CONSOLE_PRINTF("\n\n");

	SDP.Execute(15);

	CONSOLE_PRINTF("\n\n");

	MDP.Execute(20);
}

