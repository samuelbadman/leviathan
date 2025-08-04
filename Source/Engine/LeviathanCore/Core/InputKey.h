#pragma once

namespace Core
{
	class InputKey
	{
	public:
		enum class Keys : uint8_t
		{
			Backspace,
			Tab,
			Enter,
			LeftShift,
			RightShift,
			CapsLock,
			Escape,
			SpaceBar,
			PageUp,
			PageDown,
			End,
			Home,
			Insert,
			Delete,
			Left,
			Right,
			Up,
			Down,
			Zero,
			One,
			Two,
			Three,
			Four,
			Five,
			Six,
			Seven,
			Eight,
			Nine,
			A,
			B,
			C,
			D,
			E,
			F,
			G,
			H,
			I,
			J,
			K,
			L,
			M,
			N,
			O,
			P,
			Q,
			R,
			S,
			T,
			U,
			V,
			W,
			X,
			Y,
			Z,
			Numpad0,
			Numpad1,
			Numpad2,
			Numpad3,
			Numpad4,
			Numpad5,
			Numpad6,
			Numpad7,
			Numpad8,
			Numpad9,
			F1,
			F2,
			F3,
			F4,
			F5,
			F6,
			F7,
			F8,
			F9,
			F10,
			F11,
			F12,
			LeftCtrl,
			RightCtrl,
			Alt,
			LeftMouseButton,
			RightMouseButton,
			MiddleMouseButton,
			MouseWheelUpAxis,
			MouseWheelDownAxis,
			MouseXAxis,
			MouseYAxis,
			GamepadFaceButtonBottom,
			GamepadFaceButtonRight,
			GamepadFaceButtonLeft,
			GamepadFaceButtonTop,
			GamepadDPadUp,
			GamepadDPadDown,
			GamepadDPadLeft,
			GamepadDPadRight,
			GamepadLeftThumbstickButton,
			GamepadRightThumbstickButton,
			GamepadSpecialLeft,
			GamepadSpecialRight,
			GamepadLeftShoulder,
			GamepadRightShoulder,
			GamepadLeftThumbstickXAxis,
			GamepadLeftThumbstickYAxis,
			GamepadRightThumbstickXAxis,
			GamepadRightThumbstickYAxis,
			GamepadLeftThumbstickUp,
			GamepadLeftThumbstickDown,
			GamepadLeftThumbstickLeft,
			GamepadLeftThumbstickRight,
			GamepadRightThumbstickUp,
			GamepadRightThumbstickDown,
			GamepadRightThumbstickLeft,
			GamepadRightThumbstickRight,
			GamepadLeftTrigger,
			GamepadRightTrigger,
			GamepadLeftTriggerAxis,
			GamepadRightTriggerAxis,
			MAX
		};

	private:
		Core::InputKey::Keys Key;

	public:
		InputKey(Core::InputKey::Keys InKey = Core::InputKey::Keys::MAX);

		// Returns true if the key code is the code for a keyboard key otherwise, returns false
		static constexpr bool IsKeyCodeKeyboardKey(const Core::InputKey::Keys InKeyCode);
		constexpr bool IsKeyboardKey() const;

		// Returns true if the key code is the code for a mouse key otherwise, returns false
		static constexpr bool IsKeyCodeMouseKey(const Core::InputKey::Keys InKeyCode);
		constexpr bool IsMouseKey() const;

		// Returns true if the key code is the code for a gamepad key otherwise, returns false
		static constexpr bool IsKeyCodeGamepadKey(const Core::InputKey::Keys InKeyCode);
		constexpr bool IsGamepadKey() const;

		// Returns a string representation of the key
		std::string ToString() const;
	};
}