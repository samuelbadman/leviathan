#pragma once

#include "Core/Application.h"

namespace Core
{
	class Window;

	struct NotificationData;
}

class TitleApplication : public Core::Application
{
private:
	std::unique_ptr<Core::Window> AppWindow;

public:
	TitleApplication(Core::Engine& EngineInstanceRunningApplication);

private:
	void NotificationListener(const Core::NotificationData& Notification);
};