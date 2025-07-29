#pragma once

#include "Core/Application.h"
#include "Core/Delegate.h"

class TitleApplication : public Core::Application
{
private:
	Core::SingleDelegate SD = {};
	Core::MultiDelegate MD = {};

public:
	TitleApplication(Core::Engine& EngineInstanceRunningApplication);

private:
	void Print();

	virtual void Begin() override;

};