#pragma once

#include "Core/Application.h"

namespace Core
{
	struct NotificationData;
}

namespace Renderer
{
	class RendererModule;
}

class TitleApplicationWindow;

class TitleApplication : public Core::Application
{
private:
	std::unique_ptr<TitleApplicationWindow> AppWindow = nullptr;

	Renderer::RendererModule* RendererModuleInstance = nullptr;

public:
	TitleApplication(Core::Engine& EngineInstanceRunningApplication);
	~TitleApplication();

private:
	void NotificationListener(const Core::NotificationData& Notification);
};