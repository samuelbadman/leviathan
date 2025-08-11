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
