#include "Core/Platform/Gamepad.h"
#include "Core/ProgrammingTools/Bits.h"
#include "Definitions_Windows.h"
#include "Core/NotificationManager.h"

// TODO: Debug. To be removed
#include "Core/ConsoleOutput.h"

namespace
{
	namespace XInputGamepadInternals
	{
		constexpr uint16_t MaxThumbstickValue = 32767;
		constexpr uint8_t MaxTriggerValue = 255;

		Core::NotificationManager* NotificationManager = nullptr;

		uint8_t ConnectedGamepadBitflag = 0;

		// Stores the previous and current state of each gamepad together. The previous state is the first value of the pair and the current state is the second value of the pair
		std::array<std::pair<XINPUT_STATE, XINPUT_STATE>, XUSER_MAX_COUNT> States = {};

		WPARAM BuildGamepadInputMessageWParam(const uint8_t GamepadConnectionIndex, const WORD Input)
		{
			return (int32_t(GamepadConnectionIndex) | (int32_t(Input) << 16));
		}

		bool PostGamepadInputMessage(HWND FocusedWindowHandle, const WPARAM wParam, const LPARAM lParam)
		{
			return PostMessage(FocusedWindowHandle, WM_GAMEPAD_INPUT, wParam, lParam) != 0;
		}
	}
}

void Core::Gamepad::Initialize(Core::NotificationManager* pNotificationManager)
{
	XInputGamepadInternals::NotificationManager = pNotificationManager;

	XInputGamepadInternals::ConnectedGamepadBitflag = 0;
	XInputGamepadInternals::States = {};
}

void Core::Gamepad::OnRawGamepadConnectionEvent()
{
	CONSOLE_PRINTF("Gamepad connection event handled inside gamepad implementation\n");

	XINPUT_STATE DummyState;

	// For each potential gamepad connection
	for (uint8_t i = 0; i < XUSER_MAX_COUNT; ++i)
	{
		// XInputGetState() returns ERROR_SUCCESS if the function succeeds and ERROR_DEVICE_NOT_CONNECTED if the controller is not connected
		switch (XInputGetState(i, &DummyState))
		{
		case ERROR_SUCCESS:
			// Check the gamepad is not already flagged as connected
			if (!CHECK_BIT(XInputGamepadInternals::ConnectedGamepadBitflag, i))
			{
				SET_BIT(XInputGamepadInternals::ConnectedGamepadBitflag, i);

				if (XInputGamepadInternals::NotificationManager)
				{
					Core::NotificationData GamepadConnectedNotificationData = {};
					GamepadConnectedNotificationData.Type = Core::NotificationType::GamepadConnected;
					GamepadConnectedNotificationData.Payload.GamepadConnectedPayload.ConnectionIndex = i;

					XInputGamepadInternals::NotificationManager->SendNotification(GamepadConnectedNotificationData);
				}
			}
			break;

		case ERROR_DEVICE_NOT_CONNECTED:
			// Check the gamepad is not already flagged as disconnected
			if (CHECK_BIT(XInputGamepadInternals::ConnectedGamepadBitflag, i))
			{
				CLEAR_BIT(XInputGamepadInternals::ConnectedGamepadBitflag, i);

				if (XInputGamepadInternals::NotificationManager)
				{
					Core::NotificationData GamepadDisconnectedNotificationData = {};
					GamepadDisconnectedNotificationData.Type = Core::NotificationType::GamepadDisconnected;
					GamepadDisconnectedNotificationData.Payload.GamepadConnectedPayload.ConnectionIndex = i;

					XInputGamepadInternals::NotificationManager->SendNotification(GamepadDisconnectedNotificationData);
				}
			}
			break;

		default:
			break;
		}
	}

	// TODO: Debug. To be removed
	for (uint8_t i = 0; i < XUSER_MAX_COUNT; ++i)
	{
		if (CHECK_BIT(XInputGamepadInternals::ConnectedGamepadBitflag, i))
		{
			CONSOLE_PRINTF("gamepad at connection %d is connected\n", i);
		}
		else
		{
			CONSOLE_PRINTF("gamepad at connection %d is disconnected\n", i);
		}
	}
}

void Core::Gamepad::PollConnectedGamepads()
{
	// Poll
	for (uint8_t i = 0; i < XUSER_MAX_COUNT; ++i)
	{
		if (CHECK_BIT(XInputGamepadInternals::ConnectedGamepadBitflag, i))
		{
			// Gamepad is connected at index i
			// Retrieve gamepad current state 
			if (XInputGetState(static_cast<DWORD>(i), &XInputGamepadInternals::States[i].second) != ERROR_SUCCESS)
			{
				// Something went wrong. Either the gamepad is not connected and XInputGetState returned ERROR_DEVICE_NOT_CONNECTED or some other error occurred
				continue;
			}

			HWND FocusedWindow = GetFocus();
			LPARAM LParam = reinterpret_cast<LPARAM>(&XInputGamepadInternals::States[i]);

			// Analog gamepad thumbstick inputs
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_APP_LEFT_THUMB_ANALOG), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_APP_RIGHT_THUMB_ANALOG), LParam);

			// Analog gamepad trigger inputs
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_APP_LEFT_TRIGGER_ANALOG), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_APP_RIGHT_TRIGGER_ANALOG), LParam);

			// Poll gamepad button inputs
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_GAMEPAD_A), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_GAMEPAD_B), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_GAMEPAD_X), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_GAMEPAD_Y), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_GAMEPAD_DPAD_UP), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_GAMEPAD_DPAD_DOWN), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_GAMEPAD_DPAD_LEFT), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_GAMEPAD_DPAD_RIGHT), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_GAMEPAD_LEFT_THUMB), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_GAMEPAD_RIGHT_THUMB), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_GAMEPAD_BACK), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_GAMEPAD_START), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_GAMEPAD_LEFT_SHOULDER), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_GAMEPAD_RIGHT_SHOULDER), LParam);

			// Digital gamepad thumbstick inputs
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_APP_LEFT_THUMB_DIGITAL), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_APP_RIGHT_THUMB_DIGITAL), LParam);

			// Digital gamepad trigger inputs
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_APP_LEFT_TRIGGER_DIGITIAL), LParam);
			XInputGamepadInternals::PostGamepadInputMessage(FocusedWindow, XInputGamepadInternals::BuildGamepadInputMessageWParam(i, XINPUT_APP_RIGHT_TRIGGER_DIGITIAL), LParam);
		}
	}
}

void Core::Gamepad::EndGamepadPolling()
{
	// Update each connected gamepad's previous state with its current state
	for (uint8_t i = 0; i < XUSER_MAX_COUNT; ++i)
	{
		if (CHECK_BIT(XInputGamepadInternals::ConnectedGamepadBitflag, i))
		{
			XInputGamepadInternals::States[i].first = XInputGamepadInternals::States[i].second;
		}
	}
}

bool Core::Gamepad::SetForceFeedback(const uint8_t GamepadConnectionIndex, const uint16_t LeftSpeed, const uint16_t RightSpeed)
{
	// Check the connection index is valid and a gamepad is connected at the connection index
	if (GamepadConnectionIndex >= XUSER_MAX_COUNT || !CHECK_BIT(XInputGamepadInternals::ConnectedGamepadBitflag, GamepadConnectionIndex))
	{
		return false;
	}

	XINPUT_VIBRATION Vibration = {};
	Vibration.wLeftMotorSpeed = LeftSpeed;
	Vibration.wRightMotorSpeed = RightSpeed;
	return XInputSetState(static_cast<DWORD>(GamepadConnectionIndex), &Vibration) == ERROR_SUCCESS;
}
