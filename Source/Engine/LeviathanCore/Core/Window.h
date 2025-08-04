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

		// Copy of the window's unique name
		const char* UniqueName;

		void* PlatformHandle = nullptr;

	public:
		using Super = Window;

		Window(Core::Engine& Engine, const char* InUniqueName);
		virtual ~Window();

		inline const char* GetUniqueName() const { return UniqueName; }

		inline void* GetPlatformHandle() const { return PlatformHandle; }
		void SetPlatformHandle(void* InHandle);

		// Begin Window interface
		virtual void OnReceivedFocus() {};
		virtual void OnLostFocus() {};
		virtual void OnCloseSignal();
		virtual void OnDestroyed();
		// End Window interface

		void Close();
	};
}