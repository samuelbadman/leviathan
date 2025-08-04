#include "InputKey.h"

Core::InputKey::InputKey(Core::InputKey::Keys InKey)
	: Key(InKey)
{
}

constexpr bool Core::InputKey::IsKeyCodeKeyboardKey(const Core::InputKey::Keys InKeyCode)
{
	return 
		(InKeyCode == Core::InputKey::Keys::Backspace) ||
		(InKeyCode == Core::InputKey::Keys::Tab) ||
		(InKeyCode == Core::InputKey::Keys::Enter) ||
		(InKeyCode == Core::InputKey::Keys::LeftShift) ||
		(InKeyCode == Core::InputKey::Keys::RightShift) ||
		(InKeyCode == Core::InputKey::Keys::CapsLock) ||
		(InKeyCode == Core::InputKey::Keys::Escape) ||
		(InKeyCode == Core::InputKey::Keys::SpaceBar) ||
		(InKeyCode == Core::InputKey::Keys::PageUp) ||
		(InKeyCode == Core::InputKey::Keys::PageDown) ||
		(InKeyCode == Core::InputKey::Keys::End) ||
		(InKeyCode == Core::InputKey::Keys::Home) ||
		(InKeyCode == Core::InputKey::Keys::Insert) ||
		(InKeyCode == Core::InputKey::Keys::Delete) ||
		(InKeyCode == Core::InputKey::Keys::Left) ||
		(InKeyCode == Core::InputKey::Keys::Right) ||
		(InKeyCode == Core::InputKey::Keys::Up) ||
		(InKeyCode == Core::InputKey::Keys::Down) ||
		(InKeyCode == Core::InputKey::Keys::Zero) ||
		(InKeyCode == Core::InputKey::Keys::One) ||
		(InKeyCode == Core::InputKey::Keys::Two) ||
		(InKeyCode == Core::InputKey::Keys::Three) ||
		(InKeyCode == Core::InputKey::Keys::Four) ||
		(InKeyCode == Core::InputKey::Keys::Five) ||
		(InKeyCode == Core::InputKey::Keys::Six) ||
		(InKeyCode == Core::InputKey::Keys::Seven) ||
		(InKeyCode == Core::InputKey::Keys::Eight) ||
		(InKeyCode == Core::InputKey::Keys::Nine) ||
		(InKeyCode == Core::InputKey::Keys::A) ||
		(InKeyCode == Core::InputKey::Keys::B) ||
		(InKeyCode == Core::InputKey::Keys::C) ||
		(InKeyCode == Core::InputKey::Keys::D) ||
		(InKeyCode == Core::InputKey::Keys::E) ||
		(InKeyCode == Core::InputKey::Keys::F) ||
		(InKeyCode == Core::InputKey::Keys::G) ||
		(InKeyCode == Core::InputKey::Keys::H) ||
		(InKeyCode == Core::InputKey::Keys::I) ||
		(InKeyCode == Core::InputKey::Keys::J) ||
		(InKeyCode == Core::InputKey::Keys::K) ||
		(InKeyCode == Core::InputKey::Keys::L) ||
		(InKeyCode == Core::InputKey::Keys::M) ||
		(InKeyCode == Core::InputKey::Keys::N) ||
		(InKeyCode == Core::InputKey::Keys::O) ||
		(InKeyCode == Core::InputKey::Keys::P) ||
		(InKeyCode == Core::InputKey::Keys::Q) ||
		(InKeyCode == Core::InputKey::Keys::R) ||
		(InKeyCode == Core::InputKey::Keys::S) ||
		(InKeyCode == Core::InputKey::Keys::T) ||
		(InKeyCode == Core::InputKey::Keys::U) ||
		(InKeyCode == Core::InputKey::Keys::V) ||
		(InKeyCode == Core::InputKey::Keys::W) ||
		(InKeyCode == Core::InputKey::Keys::X) ||
		(InKeyCode == Core::InputKey::Keys::Y) ||
		(InKeyCode == Core::InputKey::Keys::Z) ||
		(InKeyCode == Core::InputKey::Keys::Numpad0) ||
		(InKeyCode == Core::InputKey::Keys::Numpad1) ||
		(InKeyCode == Core::InputKey::Keys::Numpad2) ||
		(InKeyCode == Core::InputKey::Keys::Numpad3) ||
		(InKeyCode == Core::InputKey::Keys::Numpad4) ||
		(InKeyCode == Core::InputKey::Keys::Numpad5) ||
		(InKeyCode == Core::InputKey::Keys::Numpad6) ||
		(InKeyCode == Core::InputKey::Keys::Numpad7) ||
		(InKeyCode == Core::InputKey::Keys::Numpad8) ||
		(InKeyCode == Core::InputKey::Keys::Numpad9) ||
		(InKeyCode == Core::InputKey::Keys::F1) ||
		(InKeyCode == Core::InputKey::Keys::F2) ||
		(InKeyCode == Core::InputKey::Keys::F3) ||
		(InKeyCode == Core::InputKey::Keys::F4) ||
		(InKeyCode == Core::InputKey::Keys::F5) ||
		(InKeyCode == Core::InputKey::Keys::F6) ||
		(InKeyCode == Core::InputKey::Keys::F7) ||
		(InKeyCode == Core::InputKey::Keys::F8) ||
		(InKeyCode == Core::InputKey::Keys::F9) ||
		(InKeyCode == Core::InputKey::Keys::F10) ||
		(InKeyCode == Core::InputKey::Keys::F11) ||
		(InKeyCode == Core::InputKey::Keys::F12) ||
		(InKeyCode == Core::InputKey::Keys::LeftCtrl) ||
		(InKeyCode == Core::InputKey::Keys::RightCtrl) ||
		(InKeyCode == Core::InputKey::Keys::Alt);
}

constexpr bool Core::InputKey::IsKeyboardKey() const
{
	return Core::InputKey::IsKeyCodeKeyboardKey(Key);
}

constexpr bool Core::InputKey::IsKeyCodeMouseKey(const Core::InputKey::Keys InKeyCode)
{
	return
		(InKeyCode == Core::InputKey::Keys::LeftMouseButton) ||
		(InKeyCode == Core::InputKey::Keys::RightMouseButton) ||
		(InKeyCode == Core::InputKey::Keys::MiddleMouseButton) ||
		(InKeyCode == Core::InputKey::Keys::MouseWheelUpAxis) ||
		(InKeyCode == Core::InputKey::Keys::MouseWheelDownAxis) ||
		(InKeyCode == Core::InputKey::Keys::MouseXAxis) ||
		(InKeyCode == Core::InputKey::Keys::MouseYAxis);
}

constexpr bool Core::InputKey::IsMouseKey() const
{
	return Core::InputKey::IsKeyCodeMouseKey(Key);
}

constexpr bool Core::InputKey::IsKeyCodeGamepadKey(const Core::InputKey::Keys InKeyCode)
{
	return 
		(InKeyCode == Core::InputKey::Keys::GamepadFaceButtonBottom) ||
		(InKeyCode == Core::InputKey::Keys::GamepadFaceButtonRight) ||
		(InKeyCode == Core::InputKey::Keys::GamepadFaceButtonLeft) ||
		(InKeyCode == Core::InputKey::Keys::GamepadFaceButtonTop) ||
		(InKeyCode == Core::InputKey::Keys::GamepadDPadUp) ||
		(InKeyCode == Core::InputKey::Keys::GamepadDPadDown) ||
		(InKeyCode == Core::InputKey::Keys::GamepadDPadLeft) ||
		(InKeyCode == Core::InputKey::Keys::GamepadDPadRight) ||
		(InKeyCode == Core::InputKey::Keys::GamepadLeftThumbstickButton) ||
		(InKeyCode == Core::InputKey::Keys::GamepadRightThumbstickButton) ||
		(InKeyCode == Core::InputKey::Keys::GamepadSpecialLeft) ||
		(InKeyCode == Core::InputKey::Keys::GamepadSpecialRight) ||
		(InKeyCode == Core::InputKey::Keys::GamepadLeftShoulder) ||
		(InKeyCode == Core::InputKey::Keys::GamepadRightShoulder) ||
		(InKeyCode == Core::InputKey::Keys::GamepadLeftThumbstickXAxis) ||
		(InKeyCode == Core::InputKey::Keys::GamepadLeftThumbstickYAxis) ||
		(InKeyCode == Core::InputKey::Keys::GamepadRightThumbstickXAxis) ||
		(InKeyCode == Core::InputKey::Keys::GamepadRightThumbstickYAxis) ||
		(InKeyCode == Core::InputKey::Keys::GamepadLeftThumbstickUp) ||
		(InKeyCode == Core::InputKey::Keys::GamepadLeftThumbstickDown) ||
		(InKeyCode == Core::InputKey::Keys::GamepadLeftThumbstickLeft) ||
		(InKeyCode == Core::InputKey::Keys::GamepadLeftThumbstickRight) ||
		(InKeyCode == Core::InputKey::Keys::GamepadRightThumbstickUp) ||
		(InKeyCode == Core::InputKey::Keys::GamepadRightThumbstickDown) ||
		(InKeyCode == Core::InputKey::Keys::GamepadRightThumbstickLeft) ||
		(InKeyCode == Core::InputKey::Keys::GamepadRightThumbstickRight) ||
		(InKeyCode == Core::InputKey::Keys::GamepadLeftTrigger) ||
		(InKeyCode == Core::InputKey::Keys::GamepadRightTrigger) ||
		(InKeyCode == Core::InputKey::Keys::GamepadLeftTriggerAxis) ||
		(InKeyCode == Core::InputKey::Keys::GamepadRightTriggerAxis);
}

constexpr bool Core::InputKey::IsGamepadKey() const
{
	return Core::InputKey::IsKeyCodeGamepadKey(Key);
}

std::string Core::InputKey::ToString() const
{
	switch (Key)
	{
	case Core::InputKey::Keys::Backspace: return "Backspace";
	case Core::InputKey::Keys::Tab: return "Tab";
	case Core::InputKey::Keys::Enter: return "Enter";
	case Core::InputKey::Keys::LeftShift: return "Left shift";
	case Core::InputKey::Keys::RightShift: return "Right shift";
	case Core::InputKey::Keys::CapsLock: return "Caps lock";
	case Core::InputKey::Keys::Escape: return "Escape";
	case Core::InputKey::Keys::SpaceBar: return "Space bar";
	case Core::InputKey::Keys::PageUp: return "Page up";
	case Core::InputKey::Keys::PageDown: return "Page down";
	case Core::InputKey::Keys::End: return "End";
	case Core::InputKey::Keys::Home: return "Home";
	case Core::InputKey::Keys::Insert: return "Insert";
	case Core::InputKey::Keys::Delete: return "Delete";
	case Core::InputKey::Keys::Left: return "Left";
	case Core::InputKey::Keys::Right: return "Right";
	case Core::InputKey::Keys::Up: return "Up";
	case Core::InputKey::Keys::Down: return "Down";
	case Core::InputKey::Keys::Zero: return "0";
	case Core::InputKey::Keys::One: return "1";
	case Core::InputKey::Keys::Two: return "2";
	case Core::InputKey::Keys::Three: return "3";
	case Core::InputKey::Keys::Four: return "4";
	case Core::InputKey::Keys::Five: return "5";
	case Core::InputKey::Keys::Six: return "6";
	case Core::InputKey::Keys::Seven: return "7";
	case Core::InputKey::Keys::Eight: return "8";
	case Core::InputKey::Keys::Nine: return "9";
	case Core::InputKey::Keys::A: return "A";
	case Core::InputKey::Keys::B: return "B";
	case Core::InputKey::Keys::C: return "C";
	case Core::InputKey::Keys::D: return "D";
	case Core::InputKey::Keys::E: return "E";
	case Core::InputKey::Keys::F: return "F";
	case Core::InputKey::Keys::G: return "G";
	case Core::InputKey::Keys::H: return "H";
	case Core::InputKey::Keys::I: return "I";
	case Core::InputKey::Keys::J: return "J";
	case Core::InputKey::Keys::K: return "K";
	case Core::InputKey::Keys::L: return "L";
	case Core::InputKey::Keys::M: return "M";
	case Core::InputKey::Keys::N: return "N";
	case Core::InputKey::Keys::O: return "O";
	case Core::InputKey::Keys::P: return "P";
	case Core::InputKey::Keys::Q: return "Q";
	case Core::InputKey::Keys::R: return "R";
	case Core::InputKey::Keys::S: return "S";
	case Core::InputKey::Keys::T: return "T";
	case Core::InputKey::Keys::U: return "U";
	case Core::InputKey::Keys::V: return "V";
	case Core::InputKey::Keys::W: return "W";
	case Core::InputKey::Keys::X: return "X";
	case Core::InputKey::Keys::Y: return "Y";
	case Core::InputKey::Keys::Z: return "Z";
	case Core::InputKey::Keys::Numpad0: return "Numpad 0";
	case Core::InputKey::Keys::Numpad1: return "Numpad 1";
	case Core::InputKey::Keys::Numpad2: return "Numpad 2";
	case Core::InputKey::Keys::Numpad3: return "Numpad 3";
	case Core::InputKey::Keys::Numpad4: return "Numpad 4";
	case Core::InputKey::Keys::Numpad5: return "Numpad 5";
	case Core::InputKey::Keys::Numpad6: return "Numpad 6";
	case Core::InputKey::Keys::Numpad7: return "Numpad 7";
	case Core::InputKey::Keys::Numpad8: return "Numpad 8";
	case Core::InputKey::Keys::Numpad9: return "Numpad 9";
	case Core::InputKey::Keys::F1: return "F1";
	case Core::InputKey::Keys::F2: return "F2";
	case Core::InputKey::Keys::F3: return "F3";
	case Core::InputKey::Keys::F4: return "F4";
	case Core::InputKey::Keys::F5: return "F5";
	case Core::InputKey::Keys::F6: return "F6";
	case Core::InputKey::Keys::F7: return "F7";
	case Core::InputKey::Keys::F8: return "F8";
	case Core::InputKey::Keys::F9: return "F9";
	case Core::InputKey::Keys::F10: return "F10";
	case Core::InputKey::Keys::F11: return "F11";
	case Core::InputKey::Keys::F12: return "F12";
	case Core::InputKey::Keys::LeftCtrl: return "Left ctrl";
	case Core::InputKey::Keys::RightCtrl: return "Right ctrl";
	case Core::InputKey::Keys::Alt: return "Alt";
	case Core::InputKey::Keys::LeftMouseButton: return "Left mouse button";
	case Core::InputKey::Keys::RightMouseButton: return "Right mouse button";
	case Core::InputKey::Keys::MiddleMouseButton: return "Middle mouse button";
	case Core::InputKey::Keys::MouseWheelUpAxis: return "Mouse wheel up axis";
	case Core::InputKey::Keys::MouseWheelDownAxis: return "Mouse wheel down axis";
	case Core::InputKey::Keys::MouseXAxis: return "Mouse X axis";
	case Core::InputKey::Keys::MouseYAxis: return "Mouse Y axis";
	case Core::InputKey::Keys::GamepadFaceButtonBottom: return "Gamepad face button bottom";
	case Core::InputKey::Keys::GamepadFaceButtonRight: return "Gamepad face button right";
	case Core::InputKey::Keys::GamepadFaceButtonLeft: return "Gamepad face button left";
	case Core::InputKey::Keys::GamepadFaceButtonTop: return "Gamepad face button top";
	case Core::InputKey::Keys::GamepadDPadUp: return "Gamepad DPad up";
	case Core::InputKey::Keys::GamepadDPadDown: return "Gamepad DPad down";
	case Core::InputKey::Keys::GamepadDPadLeft: return "Gamepad DPad left";
	case Core::InputKey::Keys::GamepadDPadRight: return "Gamepad DPad right";
	case Core::InputKey::Keys::GamepadLeftThumbstickButton: return "Gamepad left thumbstick button";
	case Core::InputKey::Keys::GamepadRightThumbstickButton: return "Gamepad right thumbstick button";
	case Core::InputKey::Keys::GamepadSpecialLeft: return "Gamepad special left";
	case Core::InputKey::Keys::GamepadSpecialRight: return "Gamepad special right";
	case Core::InputKey::Keys::GamepadLeftShoulder: return "Gamepad left shoulder";
	case Core::InputKey::Keys::GamepadRightShoulder: return "Gamepad right shoulder";
	case Core::InputKey::Keys::GamepadLeftThumbstickXAxis: return "Gamepad left thumbstick X axis";
	case Core::InputKey::Keys::GamepadLeftThumbstickYAxis: return "Gamepad left thumbstick Y axis";
	case Core::InputKey::Keys::GamepadRightThumbstickXAxis: return "Gamepad right thumbstick X axis";
	case Core::InputKey::Keys::GamepadRightThumbstickYAxis: return "Gamepad right thumbstick Y axis";
	case Core::InputKey::Keys::GamepadLeftThumbstickUp: return "Gamepad left thumbstick up";
	case Core::InputKey::Keys::GamepadLeftThumbstickDown: return "Gamepad left thumbstick down";
	case Core::InputKey::Keys::GamepadLeftThumbstickLeft: return "Gamepad left thumbstick left";
	case Core::InputKey::Keys::GamepadLeftThumbstickRight: return "Gamepad left thumbstick right";
	case Core::InputKey::Keys::GamepadRightThumbstickUp: return "Gamepad right thumbstick up";
	case Core::InputKey::Keys::GamepadRightThumbstickDown: return "Gamepad right thumbstick down";
	case Core::InputKey::Keys::GamepadRightThumbstickLeft: return "Gamepad right thumbstick left";
	case Core::InputKey::Keys::GamepadRightThumbstickRight: return "Gamepad right thumbstick right";
	case Core::InputKey::Keys::GamepadLeftTrigger: return "Gamepad left trigger";
	case Core::InputKey::Keys::GamepadRightTrigger: return "Gamepad right trigger";
	case Core::InputKey::Keys::GamepadLeftTriggerAxis: return "Gamepad left trigger axis";
	case Core::InputKey::Keys::GamepadRightTriggerAxis: return "Gamepad right trigger axis";

	case Core::InputKey::Keys::MAX:
	default:
		return "Invalid key";
	}
}
