#pragma once

#include "Core/Application.h"
#include "Core/Delegate.h"

DECLARE_SINGLE_DELEGATE(TestSingleDelegate);
DECLARE_MULTI_DELEGATE(TestMultiDelegate);

DECLARE_SINGLE_DELEGATE_OneParam(TestSingleDelegateParam, int32_t);

DECLARE_MULTI_DELEGATE_OneParam(TestMultiDelegateParam, int32_t);

class TitleApplication : public Core::Application
{
private:
	TestSingleDelegate SD = {};
	TestSingleDelegateParam SDP = {};
	TestMultiDelegate MD = {};
	TestMultiDelegateParam MDP = {};

public:
	TitleApplication(Core::Engine& EngineInstanceRunningApplication);

private:
	void Print();
	void PrintP(int32_t N);

	virtual void Begin() override;

};