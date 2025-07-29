#pragma once

#include "Core/Application.h"

namespace Core
{
	struct NotificationData;
}

class TitleApplication : public Core::Application
{
public:
	TitleApplication(Core::Engine& EngineInstanceRunningApplication);

private:
	void NotificationListener(const Core::NotificationData& Notification);
};