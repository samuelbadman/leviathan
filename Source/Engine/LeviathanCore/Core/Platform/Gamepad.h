#pragma once

namespace Core
{
	class NotificationManager;

	namespace Gamepad
	{
		void Initialize(Core::NotificationManager* pNotificationManager);
		void OnRawGamepadConnectionEvent();
	}
}