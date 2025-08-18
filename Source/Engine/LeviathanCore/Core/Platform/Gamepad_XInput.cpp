#include "Gamepad.h"
#include "Core/ProgrammingTools/Bits.h"
#include "Core/NotificationManager.h"

// TODO: Debug. To be removed
#include "Core/ConsoleOutput.h"

namespace
{
	namespace XInputGamepadInternals
	{
		Core::NotificationManager* NotificationManager = nullptr;

		uint8_t ConnectedGamepadBitflag = 0;
		std::array<XINPUT_STATE, XUSER_MAX_COUNT> PreviousStates = {};
		std::array<XINPUT_STATE, XUSER_MAX_COUNT> CurrentStates = {};

		void PollGamepadButton(const uint8_t GamepadConnectionIndex, const WORD Button)
		{
			if (Button != XINPUT_GAMEPAD_A &&
				Button != XINPUT_GAMEPAD_B &&
				Button != XINPUT_GAMEPAD_X &&
				Button != XINPUT_GAMEPAD_Y &&
				Button != XINPUT_GAMEPAD_DPAD_UP &&
				Button != XINPUT_GAMEPAD_DPAD_DOWN &&
				Button != XINPUT_GAMEPAD_DPAD_LEFT &&
				Button != XINPUT_GAMEPAD_DPAD_RIGHT &&
				Button != XINPUT_GAMEPAD_LEFT_THUMB &&
				Button != XINPUT_GAMEPAD_RIGHT_THUMB &&
				Button != XINPUT_GAMEPAD_BACK &&
				Button != XINPUT_GAMEPAD_START &&
				Button != XINPUT_GAMEPAD_LEFT_SHOULDER &&
				Button != XINPUT_GAMEPAD_RIGHT_SHOULDER)
			{
				return;
			}

			const int32_t PreviousButtonState = XInputGamepadInternals::PreviousStates[GamepadConnectionIndex].Gamepad.wButtons & Button;
			const int32_t CurrentButtonState = XInputGamepadInternals::CurrentStates[GamepadConnectionIndex].Gamepad.wButtons & Button;

			if (CurrentButtonState != 0)
			{
				// Button pressed
				CONSOLE_PRINTF("Gamepad connection %d button pressed. Was repeat: %d\n", GamepadConnectionIndex, PreviousButtonState == CurrentButtonState);
				//PostMessage(GetFocus(), my message, params..., params...)
				// TODO: Win32 message. Data: Connection index, pressed/released, was repeat
			}
			else
			{
				if (PreviousButtonState != CurrentButtonState)
				{
					// Button released
					CONSOLE_PRINTF("Gamepad connection %d button released\n", GamepadConnectionIndex);
					// TODO: Win32 message. Data: Connection index, button, pressed/released, was repeat
				}
			}
		}

		float NormalizeThumbstickAxisValue(const SHORT Value)
		{
			static constexpr uint16_t MaxAnalogStickValue = 32767;
			return (std::max(-1.0f, Value / static_cast<float>(MaxAnalogStickValue)));
		}

		void PollThumbstick_Analog(const uint8_t GamepadConnectionIndex)
		{
			static constexpr uint16_t MaxAnalogStickValue = 32767;
			const float NormalizedAxisValue = (std::max(-1.0f, 
				XInputGamepadInternals::CurrentStates[GamepadConnectionIndex].Gamepad.sThumbLX / static_cast<float>(MaxAnalogStickValue)));
		}
	}
}

void Core::Gamepad::Initialize(Core::NotificationManager* pNotificationManager)
{
	XInputGamepadInternals::NotificationManager = pNotificationManager;

	XInputGamepadInternals::ConnectedGamepadBitflag = 0;
	XInputGamepadInternals::PreviousStates = {};
	XInputGamepadInternals::CurrentStates = {};
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
			if (XInputGetState(static_cast<DWORD>(i), &XInputGamepadInternals::CurrentStates[i]) != ERROR_SUCCESS)
			{
				// Something went wrong. Either the gamepad is not connected and XInputGetState returned ERROR_DEVICE_NOT_CONNECTED or some other error occurred
				continue;
			}

			// Poll gamepad button inputs

			//GamepadFaceButtonBottom = XINPUT_GAMEPAD_A,
			//	GamepadFaceButtonRight = XINPUT_GAMEPAD_B,
			//	GamepadFaceButtonLeft = XINPUT_GAMEPAD_X,
			//	GamepadFaceButtonTop = XINPUT_GAMEPAD_Y,
			//	GamepadDPadUp = XINPUT_GAMEPAD_DPAD_UP,
			//	GamepadDPadDown = XINPUT_GAMEPAD_DPAD_DOWN,
			//	GamepadDPadLeft = XINPUT_GAMEPAD_DPAD_LEFT,
			//	GamepadDPadRight = XINPUT_GAMEPAD_DPAD_RIGHT,
			//	GamepadLeftThumbstickButton = XINPUT_GAMEPAD_LEFT_THUMB,
			//	GamepadRightThumbstickButton = XINPUT_GAMEPAD_RIGHT_THUMB,
			//	GamepadSpecialLeft = XINPUT_GAMEPAD_BACK,
			//	GamepadSpecialRight = XINPUT_GAMEPAD_START,
			//	GamepadLeftShoulder = XINPUT_GAMEPAD_LEFT_SHOULDER,
			//	GamepadRightShoulder = XINPUT_GAMEPAD_RIGHT_SHOULDER,

			XInputGamepadInternals::PollGamepadButton(i, XINPUT_GAMEPAD_A);
			// TODO: Do above for each gamepad button

			// Analog gamepad thumbstick inputs
			const float NormalizedLeftThumbstickXAxisValue = XInputGamepadInternals::NormalizeThumbstickAxisValue(XInputGamepadInternals::CurrentStates[i].Gamepad.sThumbLX);
			// TODO: win32 message. Data: Connection index, thumbstick/axis, float axis value
			//TODO: Above for each analog thumbstick axis 



			// Update gamepad previous state
			XInputGamepadInternals::PreviousStates[i] = XInputGamepadInternals::CurrentStates[i];
		}
	}


}
