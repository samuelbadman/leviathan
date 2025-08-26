#pragma once

namespace Core
{
	class Module
	{
	public:
		using Super = Module;

		virtual ~Module() = default;

		// Begin module interface
		virtual void Begin() {};
		virtual void FixedTick(float FixedTimestep) {};
		virtual void Tick(double DeltaSeconds) {};
		virtual void End() {};
		// End module interface
	};
}