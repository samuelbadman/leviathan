#pragma once

#include "Application.h"

namespace Core
{
	struct NotificationData;
	struct WindowResizedDelegateParameters;
}

namespace RenderingAbstraction
{
	class RenderingAbstractionModule;

	namespace RenderHardwareInterface
	{
		struct Context;
		struct Buffer;
		struct Shader;
		struct Pipeline;
		struct Texture;
	}
}

namespace TextureImporter
{
	class TextureImporterModule;
}

struct ShaderConstants
{
	float VertexPositionOffset[3] = {0.0f, 0.0f, 0.0f};
};

class TitleApplicationWindow;

class TitleApplication : public Core::Application
{
private:
	std::unique_ptr<TitleApplicationWindow> MainAppWindow = nullptr;

	RenderingAbstraction::RenderingAbstractionModule* RenderingAbstractionModuleInstance = nullptr;
	TextureImporter::TextureImporterModule* TextureImporterModuleInstance = nullptr;

	RenderingAbstraction::RenderHardwareInterface::Context* MainAppWindowRenderContext = nullptr;
	RenderingAbstraction::RenderHardwareInterface::Buffer* VertexBuffer = nullptr;
	RenderingAbstraction::RenderHardwareInterface::Buffer* IndexBuffer = nullptr;
	RenderingAbstraction::RenderHardwareInterface::Shader* PipelineVertexShader = nullptr;
	RenderingAbstraction::RenderHardwareInterface::Shader* PipelinePixelShader = nullptr;
	RenderingAbstraction::RenderHardwareInterface::Pipeline* Pipeline = nullptr;
	RenderingAbstraction::RenderHardwareInterface::Buffer* ShaderConstantsConstantBuffer = nullptr;
	RenderingAbstraction::RenderHardwareInterface::Texture* WallTexture = nullptr;
	ShaderConstants Constants = {};
	size_t VertexStrideBytes = 0;
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