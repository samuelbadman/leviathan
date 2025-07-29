#pragma once

namespace Core
{
	class NotificationManager;

	namespace Platform
	{
		bool Initialize(Core::NotificationManager* pNotificationManager);
		void PerFrameUpdate();
		double GetFrameMicroseconds();
		bool CreateConsole();
		bool RemoveConsole();
	}
}