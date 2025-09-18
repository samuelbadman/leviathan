#pragma once

#include "Core/Application.h"

namespace Core
{
	struct NotificationData;
}

namespace Rendering
{
	class RenderingModule;

	namespace RenderHardwareInterface
	{
		struct Context;
	}
}

class TitleApplicationWindow;

class TitleApplication : public Core::Application
{
private:
	// TODO: Bind to window resize event to render app on resize
	std::unique_ptr<TitleApplicationWindow> MainAppWindow = nullptr;

	Rendering::RenderingModule* RenderingModuleInstance = nullptr;
	Rendering::RenderHardwareInterface::Context* MainAppWindowRenderContext = nullptr;

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