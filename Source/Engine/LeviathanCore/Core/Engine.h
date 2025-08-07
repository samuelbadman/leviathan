#pragma once

namespace Core
{
	class Application;
	class NotificationManager;
	class Window;

	struct WindowCreateParameters;

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
		bool SignalRestart = false;
		bool RunningApplicationInstance = false;
		double FixedTimeAccumulationSeconds = 0.0;

	public:
		Engine();
		~Engine();

		// Returns true if the engine should restart after BeginApplication() returns otherwise, returns false if the process should exit
		bool BeginApplication(std::unique_ptr<Core::Application> pApplication);

		// Returns false if the console could not be created otherwise, returns true. 
		bool CreateConsoleWindow();

		// Returns false if the console could not be freed otherwise, returns true
		bool RemoveConsoleWindow();

		// Returns a pointer to the instance of the created window if succesful otherwise, returns null
		template<class T>
		std::unique_ptr<T> CreateWindowOnPlatform(const Core::WindowCreateParameters& Parameters)
		{
			// Allocate temporary result
			std::unique_ptr<T> Temp = std::make_unique<T>(*this, Parameters.UniqueWindowName);

			// Set the window's mode flag
			Temp->SetModeFlag(Parameters.Mode);

			// Create platform implementation window
			if (!CallPlatformCreateWindowImplementation(*Temp, Parameters))
			{
				return nullptr;
			}

			// Return temporary window object
			return Temp;
		}

		// Returns true if succesful otherwise, returns false. The window being destroyed must have been created on the same thread that is calling DestroyWindowOnPlatform()
		bool DestroyWindowOnPlatform(Core::Window& WindowToDestroy);

		// Returns true if the window was succesfully made fullscreen otherwise, returns false
		bool MakeWindowFullscreenOnPlatform(Core::Window& WindowToMakeFullscreen);

		// Returns true if the window succesfully left fullscreen otherwise, returns false
		bool ExitWindowFullscreenOnPlatform(Core::Window& WindowToExitFullscreen);

		Core::NotificationManager& GetNotificationManager();
		void Quit(bool RestartEngine = false);

	private:
		void BeginApplicationMainLoop();
		void FixedTickApplication(double FrameDeltaSeconds);
		void TickApplication(double FrameDeltaSeconds);

		// This function is used to hide the platform implementation include inside the translation unit for Engine stopping it being exposed to all code that uses the engine class
		bool CallPlatformCreateWindowImplementation(Core::Window& Temp, const Core::WindowCreateParameters& Parameters);
	};
}