#pragma once

namespace Core
{
	enum class WindowMode : uint8_t;

	class Application;
	class NotificationManager;
	class Window;
	class Module;

	struct WindowCreateParameters;
	struct Rectangle;
	struct NotificationData;

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
		std::vector<std::unique_ptr<Core::Module>> ModuleInstances = {};
		bool SignalRestart = false;
		bool RunningApplicationInstance = false;
		double FixedTimeAccumulationSeconds = 0.0;

	public:
		Engine();
		~Engine();

		// Returns true if the engine should restart after BeginApplication() returns otherwise, returns false if the process should exit
		bool BeginApplication(std::unique_ptr<Core::Application> pApplication);

		// Returns false if the console could not be created otherwise, returns true. 
		bool CreateConsoleWindowOnPlatform() const;

		// Returns false if the console could not be freed otherwise, returns true
		bool RemoveConsoleWindowOnPlatform() const;

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
		bool DestroyWindowOnPlatform(Core::Window& WindowToDestroy) const;

		// Returns true if the window was succesfully made fullscreen otherwise, returns false
		bool MakeWindowFullscreenOnPlatform(Core::Window& WindowToMakeFullscreen) const;

		// Returns true if the window succesfully left fullscreen otherwise, returns false
		bool ExitWindowFullscreenOnPlatform(Core::Window& WindowToExitFullscreen) const;

		// Captures the cursor inside the rectangle dimensions preventing from moving outside of the region. Returns true if the cursor is succesfully capture
		// otherwise, returns false
		bool CaptureCursorOnPlatform(Core::Rectangle& CaptureRegion) const;

		// Uncaptures the cursor. Returns true if succesful otherwise false
		bool UncaptureCursorOnPlatform() const;

		// Returns the rectangle region the window covers describing the window's top left position and width and height
		Core::Rectangle GetWindowRegionOnPlatform(const Core::Window& TargetWindow) const;

		// Returns the rectangle region the client area of the window covers describing the window's client area top left position and width and height. The client area 
		// is the part of the window that display's output such as graphics
		Core::Rectangle GetWindowClientRegionOnPlatform(const Core::Window& TargetWindow) const;

		// Sets the position of the cursor in screen space relative to the entire monitor screen. Returns true if succesful otherwise, false
		bool SetCursorPositionOnPlatform(int32_t X, int32_t Y) const;

		// Sets the position of the cursor in screen space relative to the passed window's client area. The client area is the part of the window that display's output 
		// such as graphicsReturns true if succesful otherwise, false
		bool SetCursorPositionRelativeToWindowOnPlatform(int32_t X, int32_t Y, const Core::Window& RelativeWindow) const;

		// Returns true if the target window is focused otherwise, returns false if the target window is not focused
		bool IsWindowFocusedOnPlatform(const Core::Window& TargetWindow);

		// Returns true if the target window is minimized otherwise, returns false if the target window is not minimzed
		bool IsWindowMinimizedOnPlatform(const Core::Window& TargetWindow) const;

		// Returns true if the target window's mode was succesfully changed otherwise, returns false
		bool ChangeWindowModeOnPlatform(const Core::WindowMode NewMode, Core::Window& TargetWindow) const;

		void Quit(bool RestartEngine = false);

		void SetShowMouseCursor(bool Show) const;
		void PlatformGamepadConnectionEventDetected() const;

		Core::NotificationManager& GetNotificationManager() const;

		// Creates and initializes an instance of the specified engine module that is owned and managed by the engine instance. Returns a non-owning raw pointer to the 
		// module instance
		template<class T>
		T* CreateModule()
		{
			// Create instance of module
			std::unique_ptr<T> Temp = std::make_unique<T>();
			T* TempResult = Temp.get();
			ModuleInstances.emplace_back(std::move(Temp));
			return TempResult;
		}

	private:
		void BeginApplicationMainLoop();
		void EngineBegin();
		void EngineFixedTick(double FrameDeltaSeconds);
		void EngineTick(double FrameDeltaSeconds);
		void EngineEnd();

		// This function is used to hide the platform implementation include inside the translation unit for Engine stopping it being exposed to all code that uses the engine class
		bool CallPlatformCreateWindowImplementation(Core::Window& Temp, const Core::WindowCreateParameters& Parameters) const;
	};
}