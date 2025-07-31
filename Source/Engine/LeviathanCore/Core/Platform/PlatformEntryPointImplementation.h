#pragma once

#include "PlatformEntryPointPrototype.h"
#include "Engine.h"

#define IMPLEMENT(ApplicationType)\
	PLATFORM_ENTRY_POINT_PROTOTYPE\
	{\
		bool RunEngine = true;\
		while(RunEngine)\
		{\
			std::unique_ptr<Core::Engine> EngineInstance = std::make_unique<Core::Engine>();\
			RunEngine = EngineInstance->BeginApplication(std::make_unique<ApplicationType>(*EngineInstance));\
		}\
		return 0;\
	}