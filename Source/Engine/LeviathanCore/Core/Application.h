#pragma once

#include "PlatformEntryPointPrototype.h"
#include "Engine.h"

#define IMPLEMENT(ApplicationType)\
	PLATFORM_ENTRY_POINT_PROTOTYPE\
	{\
		std::unique_ptr<Core::Engine> EngineInstance = std::make_unique<Core::Engine>();\
		EngineInstance->BeginApplication(std::make_unique<ApplicationType>(*EngineInstance));\
		return 0;\
	}

namespace Core
{
	class Application
	{
	private:
		Core::Engine& EngineInstance;

	public:
		Application(Core::Engine& EngineInstanceRunningApplication);
		virtual ~Application() {};

		virtual void Begin() {};
		virtual void Tick() {};
		virtual void FixedTick(float FixedTimestep) {};
		virtual void End() {};
	};
}