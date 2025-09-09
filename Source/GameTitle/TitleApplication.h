#pragma once

#include "Core/Application.h"

namespace Core
{
	struct NotificationData;
}

namespace Rendering
{
	class RenderingModule;
}

class TitleApplicationWindow;

struct TitleApplicationWindowResizedParameters;

class TitleApplication : public Core::Application
{
private:
	std::unique_ptr<TitleApplicationWindow> MainAppWindow = nullptr;

	Rendering::RenderingModule* RenderingModuleInstance = nullptr;

public:
	TitleApplication(Core::Engine& EngineInstanceRunningApplication);
	~TitleApplication();

private:
	void NotificationListener(const Core::NotificationData& Notification);
	void OnMainAppWindowDestroyed();
	void OnMainAppWindowResized(const TitleApplicationWindowResizedParameters& Params);
	bool InitializeMainAppWindow();
	bool InitializeRendering();
};