#pragma once

// For gamepad input events the WPARAM parameter contains the gamepad connection index and the gamepad input identifier.
// The LPARAM parameter contains a pointer to an std::pair containing the previous XINPUT_STATE and the current XINPUT_STATE.
#define WM_GAMEPAD_INPUT (WM_APP + 0)

// Custom XInput definitions used by the engine application
#define XINPUT_APP_LEFT_THUMB_DIGITAL WORD(500)
#define XINPUT_APP_LEFT_THUMB_ANALOG WORD(501)
#define XINPUT_APP_RIGHT_THUMB_DIGITAL WORD(502)
#define XINPUT_APP_RIGHT_THUMB_ANALOG WORD(503)
#define XINPUT_APP_LEFT_TRIGGER_DIGITIAL WORD(504)
#define XINPUT_APP_LEFT_TRIGGER_ANALOG WORD(505)
#define XINPUT_APP_RIGHT_TRIGGER_DIGITIAL WORD(506)
#define XINPUT_APP_RIGHT_TRIGGER_ANALOG WORD(507)