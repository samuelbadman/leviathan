#include "TitleApplication.h"
#include "TitleApplicationWindow.h"

#include "ConsoleOutput.h"
#include "NotificationManager.h"
#include "FileIOManager.h"

#include "RenderingAbstractionModule.h"
#include "RenderHardwareInterface.h"

#include "TextureImporter/TextureImporterModule.h"
#include "TextureImporter/TextureLoader.h"

IMPLEMENT(TitleApplication)

TitleApplication::TitleApplication(Core::Engine& EngineInstanceRunningApplication)
	: Super(EngineInstanceRunningApplication)
{
	// Create console output window
	GetEngine().CreateConsoleWindowOnPlatform();

	// Add title application notification listener
	GetEngine().GetNotificationManager().AddNotificationListenerMethod<TitleApplication, &TitleApplication::NotificationListener>(this);

	// Create engine module instances used by application
	TextureImporterModuleInstance = GetEngine().CreateModule<TextureImporter::TextureImporterModule>();

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
	RenderingAbstractionModuleInstance = GetEngine().CreateModule<RenderingAbstraction::RenderingAbstractionModule>();

	if (!RenderingAbstractionModuleInstance || !MainAppWindow)
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
		// X, Y, Z, R, G, B, A
		0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top right
		0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, // bottom right
		-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, // bottom left
		-0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left 
	};

	VertexStrideBytes = sizeof(float) * 7;

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
	// TODO: Add preprocessor definition defining which rendering api is being compiled to know which source files to read or build custom shader language that 
	// translates to glsl/hlsl depending on which rendering api is being compiled
	PipelineVertexShader = RenderingAbstraction::RenderHardwareInterface::NewShader(MainAppWindowRenderContext,
		RenderingAbstraction::RenderHardwareInterface::ShaderStage::Vertex,
		GetEngine().GetFileIOManager().ReadFileToString(std::string("GameTitleContent/Shaders/OpenGL/Source/VertexShader.glsl")));

	PipelinePixelShader = RenderingAbstraction::RenderHardwareInterface::NewShader(MainAppWindowRenderContext,
		RenderingAbstraction::RenderHardwareInterface::ShaderStage::Pixel,
		GetEngine().GetFileIOManager().ReadFileToString(std::string("GameTitleContent/Shaders/OpenGL/Source/PixelShader.glsl")));

	RenderingAbstraction::RenderHardwareInterface::InputVertexAttributeLayout PipelineInputVertexAttributeLayout = {};
	PipelineInputVertexAttributeLayout.AttributeDescriptions =
	{
		RenderingAbstraction::RenderHardwareInterface::InputVertexAttributeDesc
		{
			RenderingAbstraction::RenderHardwareInterface::InputVertexAttributeValueDataType::Float3,
			sizeof(float) * 7,
			0
		},

		RenderingAbstraction::RenderHardwareInterface::InputVertexAttributeDesc
		{
			RenderingAbstraction::RenderHardwareInterface::InputVertexAttributeValueDataType::Float4,
			sizeof(float) * 7,
			sizeof(float) * 3
		}
	};

	Pipeline = RenderingAbstraction::RenderHardwareInterface::NewPipeline(MainAppWindowRenderContext,
		PipelineVertexShader,
		PipelinePixelShader,
		PipelineInputVertexAttributeLayout,
		RenderingAbstraction::RenderHardwareInterface::PrimitiveTopologyType::Triangle);

	// Create pipeline constant buffers to store constant buffer
	ShaderConstantsConstantBuffer = RenderingAbstraction::RenderHardwareInterface::NewBuffer(MainAppWindowRenderContext,
		RenderingAbstraction::RenderHardwareInterface::BufferType::Constant,
		nullptr,
		sizeof(ShaderConstants));

	// Update shader constants constant buffer data
	Constants.VertexPositionOffset[0] = 0.0f;
	Constants.VertexPositionOffset[1] = 0.0f;
	Constants.VertexPositionOffset[2] = 0.0f;

	RenderingAbstraction::RenderHardwareInterface::UpdateConstantBufferData(MainAppWindowRenderContext,
		ShaderConstantsConstantBuffer,
		0,
		&Constants,
		sizeof(ShaderConstants));

	// Create texture resources
	TextureImporter::TextureLoadData WallTextureData = TextureImporterModuleInstance->GetTextureLoader().LoadTexture("GameTitleContent/Textures/Wall.jpg");
	TextureImporterModuleInstance->GetTextureLoader().FreeTextureLoadData(WallTextureData);

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

	if (!RenderingAbstraction::RenderHardwareInterface::DeleteBuffer(MainAppWindowRenderContext, ShaderConstantsConstantBuffer))
	{
		return false;
	}
	ShaderConstantsConstantBuffer = nullptr;

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
		RenderingAbstraction::RenderHardwareInterface::SetConstantBuffer(0, ShaderConstantsConstantBuffer); // Binding parameter maps to binding set when defining constant buffer in shader
		RenderingAbstraction::RenderHardwareInterface::SetPrimitiveTopology(RenderingAbstraction::RenderHardwareInterface::PrimitiveTopologyType::Triangle);

		RenderingAbstraction::RenderHardwareInterface::ClearColorBuffer(0.2f, 0.3f, 0.4f, 1.0f);

		RenderingAbstraction::RenderHardwareInterface::DrawIndexed(VertexBuffer, VertexStrideBytes, IndexBuffer, IndexCount);
	}
	RenderingAbstraction::RenderHardwareInterface::EndFrame(MainAppWindowRenderContext);

	RenderingAbstraction::RenderHardwareInterface::Present(MainAppWindowRenderContext);
}
