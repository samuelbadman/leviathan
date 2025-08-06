#pragma once

namespace Core
{
	enum class Keys : uint8_t
	{
		Backspace = 0,
		Tab,
		Enter,
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

		One,
		Two,
		Three,
		Four,
		Five,
		Six,
		Seven,
		Eight,
		Nine,
		Zero,
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
		NumpadMultiply,
		NumpadPlus,
		NumpadMinus,
		NumpadDecimal,
		NumpadDivide,
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

		NumLock,
		ScrollLock,

		LeftShift,
		RightShift,
		LeftCtrl,
		RightCtrl,
		LeftAlt,
		RightAlt,

		SemiColon,
		Equals,
		Comma,
		Dash,
		Period,
		ForwardSlash,
		Hash,
		LeftBrace,
		Backslash,
		RightBrace,
		Apostrophe,

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

		Unknown,
		MAX
	};

	class InputKey
	{
	private:
		Core::Keys Key;
		uint16_t ScanCode;

	public:
		InputKey(Core::Keys InKey = Core::Keys::Unknown, uint16_t InScanCode = 0);

		// Returns true if the key is a keyboard key otherwise, returns false
		bool IsKeyboardKey() const;

		// Returns true if the key is a mouse key otherwise, returns false
		bool IsMouseKey() const;

		// Returns true if the key is a gamepad key otherwise, returns false
		bool IsGamepadKey() const;

		// Retrieves the scan code value for the key
		inline uint16_t GetScanCode() const { return ScanCode; }

		// Returns a string representation of the key
		std::string ToString() const;

		// Overload comparison operator to allow comparing an input key object directly with a key enumeration
		bool operator==(const Core::Keys CompareKey) const;
	};
}