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
		Numpad_0,
		Numpad_1,
		Numpad_2,
		Numpad_3,
		Numpad_4,
		Numpad_5,
		Numpad_6,
		Numpad_7,
		Numpad_8,
		Numpad_9,
		Numpad_Multiply,
		Numpad_Plus,
		Numpad_Minus,
		Numpad_Decimal,
		Numpad_Divide,
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
		MouseWheelAxis,
		Mouse_X_Axis,
		Mouse_Y_Axis,

		Gamepad_FaceButton_Bottom,
		Gamepad_FaceButton_Right,
		Gamepad_FaceButton_Left,
		Gamepad_FaceButton_Top,
		Gamepad_DPad_Up,
		Gamepad_DPad_Down,
		Gamepad_DPad_Left,
		Gamepad_DPad_Right,
		Gamepad_Left_Thumbstick_Button,
		Gamepad_Right_Thumbstick_Button,
		Gamepad_Special_Left,
		Gamepad_Special_Right,
		Gamepad_Left_Shoulder,
		Gamepad_Right_Shoulder,
		Gamepad_Left_Thumbstick_X_Axis,
		Gamepad_Left_Thumbstick_Y_Axis,
		Gamepad_Right_Thumbstick_X_Axis,
		Gamepad_Right_Thumbstick_Y_Axis,
		Gamepad_Left_Thumbstick_Up,
		Gamepad_Left_Thumbstick_Down,
		Gamepad_Left_Thumbstick_Left,
		Gamepad_Left_Thumbstick_Right,
		Gamepad_Right_Thumbstick_Up,
		Gamepad_Right_Thumbstick_Down,
		Gamepad_Right_Thumbstick_Left,
		Gamepad_Right_Thumbstick_Right,
		Gamepad_Left_Trigger,
		Gamepad_Right_Trigger,
		Gamepad_Left_Trigger_Axis,
		Gamepad_Right_Trigger_Axis,

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