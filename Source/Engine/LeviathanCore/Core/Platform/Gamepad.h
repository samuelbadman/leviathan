#pragma once

namespace Core
{
	class NotificationManager;

	namespace Gamepad
	{
		void Initialize(Core::NotificationManager* pNotificationManager);
		void OnRawGamepadConnectionEvent();
		void PollConnectedGamepads();
		bool SetForceFeedback(const uint8_t GamepadConnectionIndex, const uint16_t LeftSpeed, const uint16_t RightSpeed);
	}
}