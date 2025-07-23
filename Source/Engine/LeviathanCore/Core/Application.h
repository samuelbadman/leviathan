#pragma once

#include "PlatformEntryPointImplementation.h"

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