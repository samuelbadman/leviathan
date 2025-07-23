#pragma once

namespace Core
{
	class Application;

	/*
	* Only a single instance of Engine should ever be created and it is responsible for managing low-level platform implementations that cannot be instantiated.
	* The single instance of Engine is created during the platform entry point function defined in PlatformEntryPointImplementation.h.
	*/
	class Engine
	{
	private:
		// TODO: Move config variables into an ini file stored on disk and read in during engine startup
		// Step taken every fixed tick
		static constexpr float FixedTimestep = 0.02f;

		// Duration in seconds between each fixed tick
		static constexpr double TimeElapsedBetweenFixedTicksSeconds = 1.0;

	private:
		std::unique_ptr<Application> ApplicationInstance = nullptr;
		bool RunningApplicationInstance = false;
		double TimeAccumulationSeconds = 0.0;

	public:
		void BeginApplication(std::unique_ptr<Application> pApplication);

	private:
		void BeginApplicationMainLoop();
	};
}