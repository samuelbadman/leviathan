#include "Gamepad.h"
#include "Core/ProgrammingTools/Bits.h"
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

		float CalcThumbstickAnalogValue(const SHORT Value)
		{
			return (std::max(-1.0f, Value / static_cast<float>(MaxThumbstickValue)));
		}

		int16_t CalcThumbstickDigitalValue(const SHORT Value)
		{
			return Value / MaxThumbstickValue;
		}

		float CalcTriggerAnalogValue(const BYTE Value)
		{
			return static_cast<float>(Value) / static_cast<float>(XInputGamepadInternals::MaxTriggerValue);
		}

		int8_t CalcTriggerDigitalValue(const BYTE Value)
		{
			return Value / XInputGamepadInternals::MaxTriggerValue;
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
			const float LeftThumbstickXAxisValue = XInputGamepadInternals::CalcThumbstickAnalogValue(XInputGamepadInternals::CurrentStates[i].Gamepad.sThumbLX);
			// TODO: win32 message. Data: Connection index, thumbstick/axis, float axis value
			//TODO: Above for each analog thumbstick axis 

			// Analog gamepad trigger inputs
			const float LeftTriggerAnalogValue = XInputGamepadInternals::CalcTriggerAnalogValue(XInputGamepadInternals::CurrentStates[i].Gamepad.bLeftTrigger);
			// TODO: win32 message. Data: connection index, trigger, float axis value
			// TODO: above for each trigger

			// Digital gamepad thumbstick inputs
			// Left thumbstick X axis
			{
				const int16_t PreviousDigitalValue = XInputGamepadInternals::CalcThumbstickDigitalValue(XInputGamepadInternals::PreviousStates[i].Gamepad.sThumbLX);
				const int16_t CurrentDigitalValue = XInputGamepadInternals::CalcThumbstickDigitalValue(XInputGamepadInternals::CurrentStates[i].Gamepad.sThumbLX);

				if (CurrentDigitalValue > 0)
				{
					// Pushed right/up
					CONSOLE_PRINTF("Stick pushed right, was repeat: %d\n", PreviousDigitalValue == CurrentDigitalValue);
					// TODO: win32 message. Data: Connection index, thumbstick/axis, was repeat
				}
				else if (CurrentDigitalValue < 0)
				{
					// Pushed left/down
					CONSOLE_PRINTF("Stick pushed left, was repeat: %d\n", PreviousDigitalValue == CurrentDigitalValue);
					// TODO: win32 message. Data: Connection index, thumbstick/axis, was repeat
				}
			}
			// TODO: Above for each thumbstick, x and y axis

			// Digital gamepad trigger inputs
			{
				const uint8_t PreviousDigitalValue = XInputGamepadInternals::CalcTriggerDigitalValue(XInputGamepadInternals::PreviousStates[i].Gamepad.bLeftTrigger);
				const uint8_t CurrentDigitalValue = XInputGamepadInternals::CalcTriggerDigitalValue(XInputGamepadInternals::CurrentStates[i].Gamepad.bLeftTrigger);

				if (PreviousDigitalValue != CurrentDigitalValue)
				{
					// Pressed/released
					CONSOLE_PRINTF("Trigger pressed/released: %d, was repeat %d\n", CurrentDigitalValue, 0);
				}

				if (CurrentDigitalValue == 1)
				{
					// Repeat
					CONSOLE_PRINTF("Trigger pressed/released: %d, was repeat %d\n", CurrentDigitalValue, 1);
				}
			}
			// TODO: win32 message. Data: connection id, trigger, pressed/released, was repeat
			// TODO: above for each trigger

			// Update gamepad previous state
			XInputGamepadInternals::PreviousStates[i] = XInputGamepadInternals::CurrentStates[i];
		}
	}


}
