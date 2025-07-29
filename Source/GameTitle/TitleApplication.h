#pragma once

#include "Core/Application.h"
#include "Core/Delegate.h"

DECLARE_SINGLE_DELEGATE(TestSingleDelegate);
DECLARE_MULTI_DELEGATE(TestMultiDelegate);

class TitleApplication : public Core::Application
{
private:
	TestSingleDelegate SD = {};
	TestMultiDelegate MD = {};

public:
	TitleApplication(Core::Engine& EngineInstanceRunningApplication);

private:
	void Print();

	virtual void Begin() override;

};