#pragma once

#include "Core/Application.h"
#include "Core/Delegate.h"

class TitleApplication : public Core::Application
{
private:
	Core::Delegate D = {};

public:
	TitleApplication(Core::Engine& EngineInstanceRunningApplication);

private:
	void Print();

	virtual void Begin() override;

};