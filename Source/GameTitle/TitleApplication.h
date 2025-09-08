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

struct TitleApplicationWindowResizedParameters;

class TitleApplication : public Core::Application
{
private:
	Renderer::RendererModule* RendererModuleInstance = nullptr;

	std::unique_ptr<TitleApplicationWindow> AppWindow = nullptr;
	void* AppWindowRenderingContext = nullptr;

public:
	TitleApplication(Core::Engine& EngineInstanceRunningApplication);
	~TitleApplication();

private:
	virtual void Tick(double DeltaSeconds) override;

	void NotificationListener(const Core::NotificationData& Notification);
	void OnAppWindowDestroyed();
	void OnAppWindowResized(const TitleApplicationWindowResizedParameters& Params);
};