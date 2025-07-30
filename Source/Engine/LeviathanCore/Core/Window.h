#pragma once

namespace Core
{
	class Window;

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
		void* PlatformHandle = nullptr;

	public:
		~Window();

		inline void* GetPlatformHandle() const { return PlatformHandle; }
		void SetPlatformHandle(void* InHandle);
	};
}