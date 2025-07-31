#pragma once

namespace Core
{
	class Window;
	class Engine;

	enum class WindowMode : uint8_t
	{
		Windowed,
		Borderless,
		NoResize,
		NoDragSize,
		MAX
	};

	struct WindowCreateParameters
	{
		const char* UniqueWindowName = nullptr;
		const char* WindowName = nullptr;
		Core::WindowMode Mode = WindowMode::MAX;
		int32_t HorizontalPosition = 0;
		int32_t VerticalPosition = 0;
		int32_t Width = 0;
		int32_t Height = 0;
		Core::Window* ParentWindow = nullptr;
	};

	class Window
	{
	private:
		Core::Engine& EngineInstance;

		// Copy of the parameters used to create the window instance
		const Core::WindowCreateParameters CreationParameters;

		void* PlatformHandle = nullptr;

	public:
		using Super = Window;

		Window(Core::Engine& Engine, const Core::WindowCreateParameters& InCreationParameters);
		virtual ~Window();

		inline const Core::WindowCreateParameters& GetCreationParameters() const { return CreationParameters; }

		inline void* GetPlatformHandle() const { return PlatformHandle; }
		void SetPlatformHandle(void* InHandle);

		// Begin Window interface
		virtual void OnCloseSignal();
		virtual void OnDestroyed();
		// End Window interface

		void Close();
	};
}