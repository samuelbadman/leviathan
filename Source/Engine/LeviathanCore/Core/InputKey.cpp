#include "InputKey.h"

Core::InputKey::InputKey(Core::Keys InKey, uint16_t InScanCode)
	: Key(InKey),
	ScanCode(InScanCode)
{
}

bool Core::InputKey::IsKeyboardKey() const
{
	return
		Key == Core::Keys::Backspace ||
		Key == Core::Keys::Tab ||
		Key == Core::Keys::Enter ||
		Key == Core::Keys::CapsLock ||
		Key == Core::Keys::Escape ||
		Key == Core::Keys::SpaceBar ||
		Key == Core::Keys::PageUp ||
		Key == Core::Keys::PageDown ||
		Key == Core::Keys::End ||
		Key == Core::Keys::Home ||
		Key == Core::Keys::Insert ||
		Key == Core::Keys::Delete ||
		Key == Core::Keys::Left ||
		Key == Core::Keys::Right ||
		Key == Core::Keys::Up ||
		Key == Core::Keys::Down ||
		Key == Core::Keys::One ||
		Key == Core::Keys::Two ||
		Key == Core::Keys::Three ||
		Key == Core::Keys::Four ||
		Key == Core::Keys::Five ||
		Key == Core::Keys::Six ||
		Key == Core::Keys::Seven ||
		Key == Core::Keys::Eight ||
		Key == Core::Keys::Nine ||
		Key == Core::Keys::Zero ||
		Key == Core::Keys::A ||
		Key == Core::Keys::B ||
		Key == Core::Keys::C ||
		Key == Core::Keys::D ||
		Key == Core::Keys::E ||
		Key == Core::Keys::F ||
		Key == Core::Keys::G ||
		Key == Core::Keys::H ||
		Key == Core::Keys::I ||
		Key == Core::Keys::J ||
		Key == Core::Keys::K ||
		Key == Core::Keys::L ||
		Key == Core::Keys::M ||
		Key == Core::Keys::N ||
		Key == Core::Keys::O ||
		Key == Core::Keys::P ||
		Key == Core::Keys::Q ||
		Key == Core::Keys::R ||
		Key == Core::Keys::S ||
		Key == Core::Keys::T ||
		Key == Core::Keys::U ||
		Key == Core::Keys::V ||
		Key == Core::Keys::W ||
		Key == Core::Keys::X ||
		Key == Core::Keys::Y ||
		Key == Core::Keys::Z ||
		Key == Core::Keys::Numpad0 ||
		Key == Core::Keys::Numpad1 ||
		Key == Core::Keys::Numpad2 ||
		Key == Core::Keys::Numpad3 ||
		Key == Core::Keys::Numpad4 ||
		Key == Core::Keys::Numpad5 ||
		Key == Core::Keys::Numpad6 ||
		Key == Core::Keys::Numpad7 ||
		Key == Core::Keys::Numpad8 ||
		Key == Core::Keys::Numpad9 ||
		Key == Core::Keys::NumpadMultiply ||
		Key == Core::Keys::NumpadPlus ||
		Key == Core::Keys::NumpadMinus ||
		Key == Core::Keys::NumpadDecimal ||
		Key == Core::Keys::NumpadDivide ||
		Key == Core::Keys::F1 ||
		Key == Core::Keys::F2 ||
		Key == Core::Keys::F3 ||
		Key == Core::Keys::F4 ||
		Key == Core::Keys::F5 ||
		Key == Core::Keys::F6 ||
		Key == Core::Keys::F7 ||
		Key == Core::Keys::F8 ||
		Key == Core::Keys::F9 ||
		Key == Core::Keys::F10 ||
		Key == Core::Keys::F11 ||
		Key == Core::Keys::F12 ||
		Key == Core::Keys::NumLock ||
		Key == Core::Keys::ScrollLock ||
		Key == Core::Keys::LeftShift ||
		Key == Core::Keys::RightShift ||
		Key == Core::Keys::LeftCtrl ||
		Key == Core::Keys::RightCtrl ||
		Key == Core::Keys::LeftAlt ||
		Key == Core::Keys::RightAlt ||
		Key == Core::Keys::SemiColon ||
		Key == Core::Keys::Equals ||
		Key == Core::Keys::Comma ||
		Key == Core::Keys::Dash ||
		Key == Core::Keys::Period ||
		Key == Core::Keys::ForwardSlash ||
		Key == Core::Keys::Hash ||
		Key == Core::Keys::LeftBrace ||
		Key == Core::Keys::Backslash ||
		Key == Core::Keys::RightBrace ||
		Key == Core::Keys::Apostrophe;
}

bool Core::InputKey::IsMouseKey() const
{
	return
		Key == Core::Keys::LeftMouseButton ||
		Key == Core::Keys::RightMouseButton ||
		Key == Core::Keys::MiddleMouseButton ||
		Key == Core::Keys::MouseWheelAxis ||
		Key == Core::Keys::MouseXAxis ||
		Key == Core::Keys::MouseYAxis;
}

bool Core::InputKey::IsGamepadKey() const
{
	return
		Key == Core::Keys::GamepadFaceButtonBottom ||
		Key == Core::Keys::GamepadFaceButtonRight ||
		Key == Core::Keys::GamepadFaceButtonLeft ||
		Key == Core::Keys::GamepadFaceButtonTop ||
		Key == Core::Keys::GamepadDPadUp ||
		Key == Core::Keys::GamepadDPadDown ||
		Key == Core::Keys::GamepadDPadLeft ||
		Key == Core::Keys::GamepadDPadRight ||
		Key == Core::Keys::GamepadLeftThumbstickButton ||
		Key == Core::Keys::GamepadRightThumbstickButton ||
		Key == Core::Keys::GamepadSpecialLeft ||
		Key == Core::Keys::GamepadSpecialRight ||
		Key == Core::Keys::GamepadLeftShoulder ||
		Key == Core::Keys::GamepadRightShoulder ||
		Key == Core::Keys::GamepadLeftThumbstickXAxis ||
		Key == Core::Keys::GamepadLeftThumbstickYAxis ||
		Key == Core::Keys::GamepadRightThumbstickXAxis ||
		Key == Core::Keys::GamepadRightThumbstickYAxis ||
		Key == Core::Keys::GamepadLeftThumbstickUp ||
		Key == Core::Keys::GamepadLeftThumbstickDown ||
		Key == Core::Keys::GamepadLeftThumbstickLeft ||
		Key == Core::Keys::GamepadLeftThumbstickRight ||
		Key == Core::Keys::GamepadRightThumbstickUp ||
		Key == Core::Keys::GamepadRightThumbstickDown ||
		Key == Core::Keys::GamepadRightThumbstickLeft ||
		Key == Core::Keys::GamepadRightThumbstickRight ||
		Key == Core::Keys::GamepadLeftTrigger ||
		Key == Core::Keys::GamepadRightTrigger ||
		Key == Core::Keys::GamepadLeftTriggerAxis ||
		Key == Core::Keys::GamepadRightTriggerAxis;
}

std::string Core::InputKey::ToString() const
{
	switch (Key)
	{
	case Core::Keys::Backspace: return "Backspace";
	case Core::Keys::Tab: return "Tab";
	case Core::Keys::Enter: return "Enter";
	case Core::Keys::CapsLock: return "Caps lock";
	case Core::Keys::Escape: return "Escape";
	case Core::Keys::SpaceBar: return "Space bar";
	case Core::Keys::PageUp: return "Page up";
	case Core::Keys::PageDown: return "Page down";
	case Core::Keys::End: return "End";
	case Core::Keys::Home: return "Home";
	case Core::Keys::Insert: return "Insert";
	case Core::Keys::Delete: return "Delete";
	case Core::Keys::Left: return "Left";
	case Core::Keys::Right: return "Right";
	case Core::Keys::Up: return "Up";
	case Core::Keys::Down: return "Down";

	case Core::Keys::Zero: return "0";
	case Core::Keys::One: return "1";
	case Core::Keys::Two: return "2";
	case Core::Keys::Three: return "3";
	case Core::Keys::Four: return "4";
	case Core::Keys::Five: return "5";
	case Core::Keys::Six: return "6";
	case Core::Keys::Seven: return "7";
	case Core::Keys::Eight: return "8";
	case Core::Keys::Nine: return "9";
	case Core::Keys::A: return "A";
	case Core::Keys::B: return "B";
	case Core::Keys::C: return "C";
	case Core::Keys::D: return "D";
	case Core::Keys::E: return "E";
	case Core::Keys::F: return "F";
	case Core::Keys::G: return "G";
	case Core::Keys::H: return "H";
	case Core::Keys::I: return "I";
	case Core::Keys::J: return "J";
	case Core::Keys::K: return "K";
	case Core::Keys::L: return "L";
	case Core::Keys::M: return "M";
	case Core::Keys::N: return "N";
	case Core::Keys::O: return "O";
	case Core::Keys::P: return "P";
	case Core::Keys::Q: return "Q";
	case Core::Keys::R: return "R";
	case Core::Keys::S: return "S";
	case Core::Keys::T: return "T";
	case Core::Keys::U: return "U";
	case Core::Keys::V: return "V";
	case Core::Keys::W: return "W";
	case Core::Keys::X: return "X";
	case Core::Keys::Y: return "Y";
	case Core::Keys::Z: return "Z";
	case Core::Keys::Numpad0: return "Numpad 0";
	case Core::Keys::Numpad1: return "Numpad 1";
	case Core::Keys::Numpad2: return "Numpad 2";
	case Core::Keys::Numpad3: return "Numpad 3";
	case Core::Keys::Numpad4: return "Numpad 4";
	case Core::Keys::Numpad5: return "Numpad 5";
	case Core::Keys::Numpad6: return "Numpad 6";
	case Core::Keys::Numpad7: return "Numpad 7";
	case Core::Keys::Numpad8: return "Numpad 8";
	case Core::Keys::Numpad9: return "Numpad 9";
	case Core::Keys::NumpadMultiply: return "Numpad *";
	case Core::Keys::NumpadPlus: return "Numpad +";
	case Core::Keys::NumpadMinus: return "Numpad -";
	case Core::Keys::NumpadDecimal: return "Numpad .";
	case Core::Keys::NumpadDivide: return "Numpad /";
	case Core::Keys::F1: return "F1";
	case Core::Keys::F2: return "F2";
	case Core::Keys::F3: return "F3";
	case Core::Keys::F4: return "F4";
	case Core::Keys::F5: return "F5";
	case Core::Keys::F6: return "F6";
	case Core::Keys::F7: return "F7";
	case Core::Keys::F8: return "F8";
	case Core::Keys::F9: return "F9";
	case Core::Keys::F10: return "F10";
	case Core::Keys::F11: return "F11";
	case Core::Keys::F12: return "F12";

	case Core::Keys::NumLock: return "Num lock";
	case Core::Keys::ScrollLock: return "Scroll lock";

	case Core::Keys::LeftShift: return "Left shift";
	case Core::Keys::RightShift: return "Right shift";
	case Core::Keys::LeftCtrl: return "Left ctrl";
	case Core::Keys::RightCtrl: return "Right ctrl";
	case Core::Keys::LeftAlt: return "Left alt";
	case Core::Keys::RightAlt: return "Right alt";

	case Core::Keys::SemiColon: return "Semicolon/colon";
	case Core::Keys::Equals: return "Equals/plus";
	case Core::Keys::Comma: return "Comma/left angular bracket";
	case Core::Keys::Dash: return "Dash/underscore";
	case Core::Keys::Period: return "Period/right angular bracket";
	case Core::Keys::ForwardSlash: return "Forward slash/question mark";
	case Core::Keys::Hash: return "hash/tilde";
	case Core::Keys::LeftBrace: return "Left square/curly brace";
	case Core::Keys::Backslash: return "Backwards slash/pipe";
	case Core::Keys::RightBrace: return "Right square/curly brace";
	case Core::Keys::Apostrophe: return "Apostrophe/@";

	case Core::Keys::LeftMouseButton: return "Left mouse button";
	case Core::Keys::RightMouseButton: return "Right mouse button";
	case Core::Keys::MiddleMouseButton: return "Middle mouse button";
	case Core::Keys::MouseWheelAxis: return "Mouse wheel axis";
	case Core::Keys::MouseXAxis: return "Mouse X axis";
	case Core::Keys::MouseYAxis: return "Mouse Y axis";

	case Core::Keys::GamepadFaceButtonBottom: return "Gamepad face button bottom";
	case Core::Keys::GamepadFaceButtonRight: return "Gamepad face button right";
	case Core::Keys::GamepadFaceButtonLeft: return "Gamepad face button left";
	case Core::Keys::GamepadFaceButtonTop: return "Gamepad face button top";
	case Core::Keys::GamepadDPadUp: return "Gamepad DPad up";
	case Core::Keys::GamepadDPadDown: return "Gamepad DPad down";
	case Core::Keys::GamepadDPadLeft: return "Gamepad DPad left";
	case Core::Keys::GamepadDPadRight: return "Gamepad DPad right";
	case Core::Keys::GamepadLeftThumbstickButton: return "Gamepad left thumbstick button";
	case Core::Keys::GamepadRightThumbstickButton: return "Gamepad right thumbstick button";
	case Core::Keys::GamepadSpecialLeft: return "Gamepad special left";
	case Core::Keys::GamepadSpecialRight: return "Gamepad special right";
	case Core::Keys::GamepadLeftShoulder: return "Gamepad left shoulder";
	case Core::Keys::GamepadRightShoulder: return "Gamepad right shoulder";
	case Core::Keys::GamepadLeftThumbstickXAxis: return "Gamepad left thumbstick X axis";
	case Core::Keys::GamepadLeftThumbstickYAxis: return "Gamepad left thumbstick Y axis";
	case Core::Keys::GamepadRightThumbstickXAxis: return "Gamepad right thumbstick X axis";
	case Core::Keys::GamepadRightThumbstickYAxis: return "Gamepad right thumbstick Y axis";
	case Core::Keys::GamepadLeftThumbstickUp: return "Gamepad left thumbstick up";
	case Core::Keys::GamepadLeftThumbstickDown: return "Gamepad left thumbstick down";
	case Core::Keys::GamepadLeftThumbstickLeft: return "Gamepad left thumbstick left";
	case Core::Keys::GamepadLeftThumbstickRight: return "Gamepad left thumbstick right";
	case Core::Keys::GamepadRightThumbstickUp: return "Gamepad right thumbstick up";
	case Core::Keys::GamepadRightThumbstickDown: return "Gamepad right thumbstick down";
	case Core::Keys::GamepadRightThumbstickLeft: return "Gamepad right thumbstick left";
	case Core::Keys::GamepadRightThumbstickRight: return "Gamepad right thumbstick right";
	case Core::Keys::GamepadLeftTrigger: return "Gamepad left trigger";
	case Core::Keys::GamepadRightTrigger: return "Gamepad right trigger";
	case Core::Keys::GamepadLeftTriggerAxis: return "Gamepad left trigger axis";
	case Core::Keys::GamepadRightTriggerAxis: return "Gamepad right trigger axis";

	case Core::Keys::Unknown: return "Unknown key";

	case Core::Keys::MAX:
	default:
		return "Invalid key";
	}
}

bool Core::InputKey::operator==(const Core::Keys CompareKey) const
{
	return (Key == CompareKey);
}
