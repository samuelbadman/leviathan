#include "TitleApplication.h"
#include "Core/ConsoleOutput.h"
#include "Core/NotificationManager.h"
#include "Core/FileIOManager.h"
#include "TitleApplicationWindow.h"
#include "RenderingAbstraction/RenderingAbstractionModule.h"
#include "RenderingAbstraction/RenderHardwareInterface/RenderHardwareInterface.h"

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
	RenderingModuleInstance = GetEngine().CreateModule<RenderingAbstraction::RenderingAbstractionModule>();

	if (!RenderingModuleInstance || !MainAppWindow)
	{
		return false;
	}

	// Initialize render hardware interface
	if (!RenderingAbstraction::RenderHardwareInterface::Initialize(MainAppWindow->GetPlatformHandle()))
	{
		return false;
	}

	// Create render context for main app window
	MainAppWindowRenderContext = RenderingAbstraction::RenderHardwareInterface::NewContext(MainAppWindow->GetPlatformHandle());
	if (!MainAppWindowRenderContext)
	{
		return false;
	}

	// Initialize rendering scene
	std::vector<float> Vertices =
	{
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};

	VertexStrideBytes = sizeof(float) * 3;

	std::vector<uint32_t> Indices =
	{
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	IndexCount = Indices.size();

	VertexBuffer = RenderingAbstraction::RenderHardwareInterface::NewBuffer(MainAppWindowRenderContext,
		RenderingAbstraction::RenderHardwareInterface::BufferType::Vertex,
		Vertices.data(),
		Vertices.size() * sizeof(float));

	IndexBuffer = RenderingAbstraction::RenderHardwareInterface::NewBuffer(MainAppWindowRenderContext,
		RenderingAbstraction::RenderHardwareInterface::BufferType::Index,
		Indices.data(),
		Indices.size() * sizeof(uint32_t));

	// Create app rendering pipelines
	// TODO: Add preprocessor definition defining which rendering api is being compiled
	PipelineVertexShader = RenderingAbstraction::RenderHardwareInterface::NewShader(MainAppWindowRenderContext,
		RenderingAbstraction::RenderHardwareInterface::ShaderStage::Vertex,
		GetEngine().GetFileIOManager().ReadDiskFileToString(std::string("Shaders/VertexShader.glsl")));

	PipelinePixelShader = RenderingAbstraction::RenderHardwareInterface::NewShader(MainAppWindowRenderContext,
		RenderingAbstraction::RenderHardwareInterface::ShaderStage::Pixel,
		GetEngine().GetFileIOManager().ReadDiskFileToString(std::string("Shaders/PixelShader.glsl")));

	RenderingAbstraction::RenderHardwareInterface::InputVertexAttributeLayout PipelineInputVertexAttributeLayout = {};
	PipelineInputVertexAttributeLayout.AttributeDescriptions =
	{
		RenderingAbstraction::RenderHardwareInterface::InputVertexAttributeDesc
		{
			RenderingAbstraction::RenderHardwareInterface::InputVertexAttributeValueDataType::Float3, 
			sizeof(float) * 3, 
			0
		}
	};

	Pipeline = RenderingAbstraction::RenderHardwareInterface::NewPipeline(MainAppWindowRenderContext, 
		PipelineVertexShader,
		PipelinePixelShader,
		PipelineInputVertexAttributeLayout,
		RenderingAbstraction::RenderHardwareInterface::PrimitiveTopologyType::Triangle);

	return true;
}

bool TitleApplication::ShutdownRendering()
{
	// Delete rendering resources
	if (!RenderingAbstraction::RenderHardwareInterface::DeleteBuffer(MainAppWindowRenderContext, VertexBuffer))
	{
		return false;
	}
	VertexBuffer = nullptr;

	if (!RenderingAbstraction::RenderHardwareInterface::DeleteBuffer(MainAppWindowRenderContext, IndexBuffer))
	{
		return false;
	}
	IndexBuffer = nullptr;

	if (!RenderingAbstraction::RenderHardwareInterface::DeleteShader(MainAppWindowRenderContext, PipelineVertexShader))
	{
		return false;
	}
	PipelineVertexShader = nullptr;

	if (!RenderingAbstraction::RenderHardwareInterface::DeleteShader(MainAppWindowRenderContext, PipelinePixelShader))
	{
		return false;
	}
	PipelinePixelShader = nullptr;

	if (!RenderingAbstraction::RenderHardwareInterface::DeletePipeline(MainAppWindowRenderContext, Pipeline))
	{
		return false;
	}
	Pipeline = nullptr;

	// Delete main app window rendering context
	if (!RenderingAbstraction::RenderHardwareInterface::DeleteContext(MainAppWindowRenderContext))
	{
		return false;
	}
	MainAppWindowRenderContext = nullptr;

	// Shutdown rhi
	if (!RenderingAbstraction::RenderHardwareInterface::Shutdown())
	{
		return false;
	}

	return true;
}

void TitleApplication::RenderApp()
{
	RenderingAbstraction::RenderHardwareInterface::BeginFrame(MainAppWindowRenderContext);
	{
		const Core::Rectangle WindowClientRect = MainAppWindow->GetClientRegion();
		RenderingAbstraction::RenderHardwareInterface::SetViewport(0, 0, WindowClientRect.CalcWidth(), WindowClientRect.CalcHeight());
		RenderingAbstraction::RenderHardwareInterface::SetPipeline(Pipeline);

		RenderingAbstraction::RenderHardwareInterface::ClearColorBuffer(0.2f, 0.3f, 0.4f, 1.0f);

		RenderingAbstraction::RenderHardwareInterface::DrawIndexed(VertexBuffer, VertexStrideBytes, IndexBuffer, IndexCount);
	}
	RenderingAbstraction::RenderHardwareInterface::EndFrame(MainAppWindowRenderContext);

	RenderingAbstraction::RenderHardwareInterface::Present(MainAppWindowRenderContext);
}
