#pragma once

#include "InputKey.h"
#include "ProgrammingTools/Delegate.h"

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

	enum class InputEvent : uint8_t
	{
		Pressed,
		Repeat,
		Released,
		DoubleClick,
		Axis,
		MAX
	};

	struct WindowCreateParameters
	{
		const char* UniqueWindowName = nullptr;
		const char* WindowTitle = nullptr;
		Core::WindowMode Mode = WindowMode::MAX;
		int32_t HorizontalPosition = 0;
		int32_t VerticalPosition = 0;
		int32_t Width = 0;
		int32_t Height = 0;
		Core::Window* ParentWindow = nullptr;
	};

	struct InputEventArgs
	{
		Core::InputKey Key = {};
		Core::InputEvent Event = Core::InputEvent::MAX;
		float Data = 0.0f;
	};

	struct Rectangle
	{
		int32_t Top = 0;
		int32_t Left = 0;
		int32_t Bottom = 0;
		int32_t Right = 0;

		int32_t CalcWidth() const;
		int32_t CalcHeight() const;
	};

	struct WindowResizedDelegateParameters
	{
		Window* pWindow = nullptr;
		uint32_t NewWidth = 0;
		uint32_t NewHeight = 0;
	};

	DECLARE_MULTI_DELEGATE_OneParam(WindowResizedSignature, const WindowResizedDelegateParameters& /* Parameters */);
	DECLARE_MULTI_DELEGATE(WindowDestroyedSignature);

	class Window
	{
	private:
		Core::Engine& EngineInstance;

		WindowResizedSignature ResizedDelegate = {};
		WindowDestroyedSignature DestroyedDelegate = {};

		// Copy of the window's unique name
		const char* UniqueName;

		void* PlatformHandle = nullptr;
		bool FullscreenFlag = false;
		Core::WindowMode ModeFlag = Core::WindowMode::MAX;
		std::array<int32_t, 2> TopLeftCoordOnEnterFullscreen = { 0, 0 };
		std::array<int32_t, 2> DimensionsOnEnterFullscreen = { 0, 0 };

	public:
		using Super = Window;

		Window(Core::Engine& Engine, const char* InUniqueName);
		virtual ~Window();

		inline const char* GetUniqueName() const { return UniqueName; }

		inline void* GetPlatformHandle() const { return PlatformHandle; }
		void SetPlatformHandle(void* InHandle);

		inline bool IsFullscreen() const { return FullscreenFlag; }
		void SetFullscreenFlag(const bool NewFullscreen);

		inline Core::WindowMode GetModeFlag() const { return ModeFlag; }
		void SetModeFlag(const Core::WindowMode NewMode);

		void GetTopLeftCoordOnEnterFullscreen(int32_t& X, int32_t& Y) const;
		void SetTopLeftCoordOnEnterFullscreen(const int32_t X, const int32_t Y);

		void GetDimensionsOnEnterFullscreen(int32_t& Width, int32_t& Height) const;
		void SetDimensionsOnEnterFullscreen(const int32_t Width, const int32_t Height);

		// Begin Window interface
		virtual void OnForceClose();
		virtual void OnInputKey(const Core::InputEventArgs& EventArgs) {};
		virtual void OnInputAxis(const Core::InputEventArgs& EventArgs) {};
		virtual void OnMaximized() {};
		virtual void OnMinimized() {};
		virtual void OnResized(uint32_t NewWidth, uint32_t NewHeight);
		virtual void OnEnterFullscreen() {};
		virtual void OnExitFullscreen() {};
		virtual void OnEnterSizeMove() {};
		virtual void OnExitSizeMove() {};
		virtual void OnReceivedFocus() {};
		virtual void OnLostFocus() {};
		virtual void OnCloseSignal();
		virtual void OnDestroyed();
		// End Window interface

		void Close();
		void MakeFullscreen();
		void ExitFullscreen();
		Core::Rectangle GetRegion() const;
		Core::Rectangle GetClientRegion() const;
		void CaptureCursorInWindowRegion() const;
		void CaptureCursorInClientRegion() const;
		bool IsFocused() const;
		bool IsMinimized() const;
		bool ChangeMode(const Core::WindowMode NewMode);

		inline WindowResizedSignature& GetResizedDelegate() { return ResizedDelegate; }
		inline WindowDestroyedSignature& GetDestroyedDelegate() { return DestroyedDelegate; }

	protected:
		inline Core::Engine& GetEngine() const { return EngineInstance; }
	};
}