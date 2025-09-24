#include "TitleApplication.h"
#include "Core/ConsoleOutput.h"
#include "Core/NotificationManager.h"
#include "Core/FileIOManager.h"
#include "TitleApplicationWindow.h"
#include "Rendering/RenderingModule.h"
#include "Rendering/RenderHardwareInterface/RenderHardwareInterface.h"

IMPLEMENT(TitleApplication)

TitleApplication::TitleApplication(Core::Engine& EngineInstanceRunningApplication)
	: Super(EngineInstanceRunningApplication)
{
	// Create console output window
	GetEngine().CreateConsoleWindowOnPlatform();

	// Add title application notification listener
	GetEngine().GetNotificationManager().AddNotificationListenerMethod<TitleApplication, &TitleApplication::NotificationListener>(this);

	// Create a main window for the title application
	InitializeMainAppWindow();

	// Initialize rendering for the application
	InitializeRendering();
}

TitleApplication::~TitleApplication()
{
	// Shutdown rendering for the application
	ShutdownRendering();

	// Remove console output window
	GetEngine().RemoveConsoleWindowOnPlatform();

	// Remove title application notification listener
	GetEngine().GetNotificationManager().RemoveNotificationListenerMethod<TitleApplication, &TitleApplication::NotificationListener>(this);
}

void TitleApplication::Tick(double DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Render application
	RenderApp();
}

void TitleApplication::NotificationListener(const Core::NotificationData& Notification)
{
	switch (Notification.Type)
	{
	case Core::NotificationType::GamepadConnected:
		CONSOLE_PRINTF("Gamepad connected at connection %d\n", Notification.Payload.GamepadConnectedPayload.ConnectionIndex);
		break;

	case Core::NotificationType::GamepadDisconnected:
		CONSOLE_PRINTF("Gamepad disconnected at connection %d\n", Notification.Payload.GamepadDisconnectedPayload.ConnectionIndex);
		break;

	default: break;
	}
}

void TitleApplication::OnMainAppWindowResized(const Core::WindowResizedDelegateParameters& Parameters)
{
	RenderApp();
}

void TitleApplication::OnMainAppWindowDestroyed()
{
	// Tell the engine to quit
	GetEngine().Quit();
}

bool TitleApplication::InitializeMainAppWindow()
{
	Core::WindowCreateParameters AppWindowCreateParameters = {};
	AppWindowCreateParameters.UniqueWindowName = "MainTitleAppWindow";
	AppWindowCreateParameters.WindowTitle = "Main app window";
	AppWindowCreateParameters.Mode = Core::WindowMode::Windowed;
	AppWindowCreateParameters.Width = 540;
	AppWindowCreateParameters.Height = 540;

	MainAppWindow = GetEngine().CreateWindowOnPlatform<TitleApplicationWindow>(AppWindowCreateParameters);

	if (!MainAppWindow)
	{
		return false;
	}

	// Bind to main app window resized delegate
	MainAppWindow->GetResizedDelegate().AddMethod<TitleApplication, &TitleApplication::OnMainAppWindowResized>(this);

	// Bind to main app window destroyed delegate
	MainAppWindow->GetDestroyedDelegate().AddMethod<TitleApplication, &TitleApplication::OnMainAppWindowDestroyed>(this);

	return true;
}

bool TitleApplication::InitializeRendering()
{
	// Create rendering module
	RenderingModuleInstance = GetEngine().CreateModule<Rendering::RenderingModule>();

	if (!RenderingModuleInstance || !MainAppWindow)
	{
		return false;
	}

	// Initialize render hardware interface
	if (!Rendering::RenderHardwareInterface::Initialize(MainAppWindow->GetPlatformHandle()))
	{
		return false;
	}

	// Create render context for main app window
	MainAppWindowRenderContext = Rendering::RenderHardwareInterface::NewContext(MainAppWindow->GetPlatformHandle());
	if (!MainAppWindowRenderContext)
	{
		return false;
	}

	// Create app rendering pipelines
	Rendering::RenderHardwareInterface::InputVertexAttributeLayout PipelineInputVertexAttributeLayout = {};
	PipelineInputVertexAttributeLayout.AttributeDescriptions =
	{
		Rendering::RenderHardwareInterface::InputVertexAttributeDesc{0, 3, Rendering::RenderHardwareInterface::InputVertexAttributeValueDataType::Float, sizeof(float) * 3, 0}
	};

	const std::string PipelineVertexShaderSource = GetEngine().GetFileIOManager().ReadDiskFileToString(std::string("Shaders/MeshVertexShader.glsl"));
	const std::string PipelinePixelShaderSource = GetEngine().GetFileIOManager().ReadDiskFileToString(std::string("Shaders/MeshPixelShader.glsl"));

	Pipeline = Rendering::RenderHardwareInterface::NewPipeline(MainAppWindowRenderContext, 
		PipelineVertexShaderSource,
		PipelineInputVertexAttributeLayout,
		PipelinePixelShaderSource);

	// Initialize rendering scene
	std::vector<float> Vertices =
	{
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};

	std::vector<uint32_t> Indices =
	{
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	IndexCount = Indices.size();

	VertexBuffer = Rendering::RenderHardwareInterface::NewVertexBuffer(MainAppWindowRenderContext, Vertices.data(), Vertices.size() * sizeof(float));
	IndexBuffer = Rendering::RenderHardwareInterface::NewIndexBuffer(MainAppWindowRenderContext, Indices.data(), Indices.size());

	return true;
}

bool TitleApplication::ShutdownRendering()
{
	// Delete rendering resources
	if (!Rendering::RenderHardwareInterface::DeleteVertexBuffer(MainAppWindowRenderContext, VertexBuffer))
	{
		return false;
	}
	VertexBuffer = nullptr;

	if (!Rendering::RenderHardwareInterface::DeleteIndexBuffer(MainAppWindowRenderContext, IndexBuffer))
	{
		return false;
	}
	IndexBuffer = nullptr;

	if (!Rendering::RenderHardwareInterface::DeletePipeline(MainAppWindowRenderContext, Pipeline))
	{
		return false;
	}
	Pipeline = nullptr;

	// Delete main app window rendering context
	if (!Rendering::RenderHardwareInterface::DeleteContext(MainAppWindowRenderContext))
	{
		return false;
	}
	MainAppWindowRenderContext = nullptr;

	// Shutdown rhi
	if (!Rendering::RenderHardwareInterface::Shutdown())
	{
		return false;
	}

	return true;
}

void TitleApplication::RenderApp()
{
	Rendering::RenderHardwareInterface::BeginFrame(MainAppWindowRenderContext);
	{
		const Core::Rectangle WindowClientRect = MainAppWindow->GetClientRegion();
		Rendering::RenderHardwareInterface::SetViewport(0, 0, WindowClientRect.CalcWidth(), WindowClientRect.CalcHeight());

		Rendering::RenderHardwareInterface::ClearColorBuffer(0.2f, 0.3f, 0.4f, 1.0f);

		Rendering::RenderHardwareInterface::SetPipeline(Pipeline);

		Rendering::RenderHardwareInterface::DrawIndexed(VertexBuffer, IndexBuffer, IndexCount);
	}
	Rendering::RenderHardwareInterface::EndFrame(MainAppWindowRenderContext);

	Rendering::RenderHardwareInterface::Present(MainAppWindowRenderContext);
}
