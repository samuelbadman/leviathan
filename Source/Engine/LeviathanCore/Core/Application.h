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
		virtual void FixedTick(float FixedTimestep) {};
		virtual void Tick(double DeltaSeconds) {};
		virtual void End() {};

	protected:
		inline Core::Engine& GetEngine() const { return EngineInstance; }
	};
}