#pragma once

namespace Core
{
	class Application;

	class Engine
	{
	private:
		// Engine configuration properties. TODO: Get from an ini text file on disk during engine startup

		// Seconds elapsed between fixed ticks. FixedTick is called every SliceSeconds seconds.
		static constexpr float SliceSeconds = 0.1f;

		// Timestep used in fixed tick call. Can be tweaked for a less or more precise simulation.
		static constexpr float FixedTimestep = 0.1f;

	private:
		std::unique_ptr<Application> ApplicationInstance = nullptr;
		bool RunningApplicationInstance = false;

	public:
		void BeginApplication(std::unique_ptr<Application> pApplication);

	private:
		void BeginApplicationMainLoop();
	};
}