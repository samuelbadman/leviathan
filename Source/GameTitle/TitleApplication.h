#pragma once

#include "Core/Application.h"

namespace Core
{
	struct NotificationData;
	struct WindowResizedDelegateParameters;
}

namespace Rendering
{
	class RenderingModule;

	namespace RenderHardwareInterface
	{
		struct Context;
		struct Buffer;
		struct Pipeline;
	}
}

class TitleApplicationWindow;

class TitleApplication : public Core::Application
{
private:
	std::unique_ptr<TitleApplicationWindow> MainAppWindow = nullptr;

	Rendering::RenderingModule* RenderingModuleInstance = nullptr;

	Rendering::RenderHardwareInterface::Context* MainAppWindowRenderContext = nullptr;
	Rendering::RenderHardwareInterface::Pipeline* Pipeline = nullptr;
	Rendering::RenderHardwareInterface::Buffer* VertexBuffer = nullptr;
	Rendering::RenderHardwareInterface::Buffer* IndexBuffer = nullptr;
	size_t IndexCount = 0;

public:
	TitleApplication(Core::Engine& EngineInstanceRunningApplication);
	~TitleApplication();

private:
	// Begin Application interface
	virtual void Tick(double DeltaSeconds) override;
	// End Application interface

	void NotificationListener(const Core::NotificationData& Notification);
	void OnMainAppWindowResized(const Core::WindowResizedDelegateParameters& Parameters);
	void OnMainAppWindowDestroyed();
	bool InitializeMainAppWindow();
	bool InitializeRendering();
	bool ShutdownRendering();
	void RenderApp();
};