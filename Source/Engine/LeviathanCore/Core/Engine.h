#pragma once

namespace Core
{
	class Application;
	class NotificationManager;

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
		std::unique_ptr<Core::NotificationManager> NotificationManagerInstance = nullptr;
		std::unique_ptr<Core::Application> ApplicationInstance = nullptr;
		bool RunningApplicationInstance = false;
		double FixedTimeAccumulationSeconds = 0.0;

	public:
		Engine();
		~Engine();

		void BeginApplication(std::unique_ptr<Core::Application> pApplication);

		// Returns false if the console could not be created otherwise, returns true. 
		bool CreateConsoleWindow();

		// Returns false if the console could not be freed otherwise, returns true
		bool RemoveConsoleWindow();

		Core::NotificationManager& GetNotificationManager();

	private:
		void BeginApplicationMainLoop();
		void FixedTickApplication(double FrameDeltaSeconds);
		void TickApplication(double FrameDeltaSeconds);
	};
}