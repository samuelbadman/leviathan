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

class TitleApplication : public Core::Application
{
private:
	std::unique_ptr<TitleApplicationWindow> MainAppWindow = nullptr;

	Rendering::RenderingModule* RenderingModuleInstance = nullptr;

public:
	TitleApplication(Core::Engine& EngineInstanceRunningApplication);
	~TitleApplication();

private:
	// Begin Application interface
	virtual void Tick(double DeltaSeconds) override;
	// End Application interface

	void NotificationListener(const Core::NotificationData& Notification);
	void OnMainAppWindowDestroyed();
	bool InitializeMainAppWindow();
	bool InitializeRendering();
	bool ShutdownRendering();
	void RenderApp();
};