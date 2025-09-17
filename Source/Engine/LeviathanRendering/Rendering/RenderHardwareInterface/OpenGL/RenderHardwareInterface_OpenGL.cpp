#include "Rendering/RenderHardwareInterface/RenderHardwareInterface.h"
#include "glad/glad.h"
#include "Core/ConsoleOutput.h"

namespace
{
	namespace OpenGLRHIInternals
	{
		struct OpenGL_OutputWindowResources
		{
			void* OpenGLContext = nullptr;
		};

		struct OpenGL_GraphicsPipeline
		{
			uint32_t ShaderProgramObject = 0;

			OpenGL_GraphicsPipeline() = default;
			OpenGL_GraphicsPipeline(const uint32_t InShaderProgramObjectID)
				: ShaderProgramObject(InShaderProgramObjectID)
			{
			}
		};

		struct OpenGL_MeshRenderObject
		{
			uint32_t VertexBufferObjectID = 0;

			OpenGL_MeshRenderObject() = default;
			OpenGL_MeshRenderObject(const uint32_t InVertexBufferObjectID)
				: VertexBufferObjectID(InVertexBufferObjectID)
			{
			}
		};

		// Mapping of platform window handles to output window resource structures. The window platform handle is being used as the handle/identifier to its rendering resources
		std::unordered_map<void*, OpenGL_OutputWindowResources> OutputWindowResources = {};

		// Mapping of uuid identifers to static render mesh objects
		std::unordered_map<Core::Uuid, OpenGL_MeshRenderObject> MeshRenderObjects = {};

		// Mapping of uuid identifers to rendering pipeline objects
		std::unordered_map<Core::Uuid, OpenGL_GraphicsPipeline> GraphicsPipelines = {};

		Core::Uuid MeshGraphicsPipeline = {};

		bool DoesOutputWindowResourcesExist(void* const WindowPlatformHandle)
		{
			return (OpenGLRHIInternals::OutputWindowResources.find(WindowPlatformHandle) != OpenGLRHIInternals::OutputWindowResources.end());
		}

		bool GetOutputWindowResources(void* const WindowPlatformHandle, OpenGLRHIInternals::OpenGL_OutputWindowResources& Resources)
		{
			if (DoesOutputWindowResourcesExist(WindowPlatformHandle))
			{
				Resources = OutputWindowResources.at(WindowPlatformHandle);
				return true;
			}
			Resources = {};
			return false;
		}

		bool AddOutputWindowResources(void* const WindowPlatformHandle, const OpenGLRHIInternals::OpenGL_OutputWindowResources& Resources)
		{
			if (!DoesOutputWindowResourcesExist(WindowPlatformHandle))
			{
				OpenGLRHIInternals::OutputWindowResources.emplace(WindowPlatformHandle, Resources);
				return true;
			}
			return false;
		}

		void* CreateContext(void* WindowPlatformHandle)
		{
#ifdef PLATFORM_WINDOWS
			// Get the window device context
			HDC WindowHandleToDeviceContext = GetDC(static_cast<HWND>(WindowPlatformHandle));

			// Set the pixel format in the device context
			PIXELFORMATDESCRIPTOR PFD =
			{
				sizeof(PIXELFORMATDESCRIPTOR),
				1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
				PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
				32,                   // Colordepth of the framebuffer.
				0, 0, 0, 0, 0, 0,
				0,
				0,
				0,
				0, 0, 0, 0,
				24,                   // Number of bits for the depthbuffer
				8,                    // Number of bits for the stencilbuffer
				0,                    // Number of Aux buffers in the framebuffer.
				PFD_MAIN_PLANE,
				0,
				0, 0, 0
			};

			int PixelFormatNumber = ChoosePixelFormat(WindowHandleToDeviceContext, &PFD);
			if (PixelFormatNumber == 0)
			{
				// Error
				return nullptr;
			}

			SetPixelFormat(WindowHandleToDeviceContext, PixelFormatNumber, &PFD);

			// Create opengl context
			return static_cast<void*>(wglCreateContext(WindowHandleToDeviceContext));
#else
			return nullptr;
#endif // PLATFORM_WINDOWS
		}

		bool MakeContextCurrent(void* WindowPlatformHandle, void* Context)
		{
#ifdef PLATFORM_WINDOWS
			return wglMakeCurrent(GetDC(static_cast<HWND>(WindowPlatformHandle)), static_cast<HGLRC>(Context)) == TRUE;
#else
			return false;
#endif // PLATFORM_WINDOWS
		}

		bool DeleteContext(void* WindowPlatformHandle, void* Context)
		{
#ifdef PLATFORM_WINDOWS
			if (wglMakeCurrent(GetDC(static_cast<HWND>(WindowPlatformHandle)), NULL) != TRUE)
			{
				return false;
			}

			return wglDeleteContext(static_cast<HGLRC>(Context)) == TRUE;
#else
			return false;
#endif // PLATFORM_WINDOWS
		}

		bool LoadOpenGLFunctions()
		{
			return gladLoadGL() != 0;
		}

		void PrintOpenGLVersion()
		{
			CONSOLE_PRINTF("OpenGL version and driver version: %s\n", glGetString(GL_VERSION));
		}

		bool SwapWindowBuffers(void* WindowPlatformHandle)
		{
#ifdef PLATFORM_WINDOWS
			return SwapBuffers(GetDC(static_cast<HWND>(WindowPlatformHandle))) == TRUE;
#else
#endif // PLATFORM_WINDOWS
		}

		bool ClearCurrentContext()
		{
#ifdef PLATFORM_WINDOWS
			return OpenGLRHIInternals::MakeContextCurrent(nullptr, nullptr);
#else
			return false;
#endif // PLATFORM_WINDOWS
		}

		bool FreeOutputWindowResources(void* const OutputWindowPlatformHandle, const OpenGLRHIInternals::OpenGL_OutputWindowResources& Resources)
		{
			// Delete rendering context for the output window
			if (!OpenGLRHIInternals::DeleteContext(OutputWindowPlatformHandle, Resources.OpenGLContext))
			{
				return false;
			}
			return true;
		}

		void DestroyMeshRenderObjectResources(const OpenGLRHIInternals::OpenGL_MeshRenderObject& RenderMesh)
		{
			glDeleteBuffers(1, &RenderMesh.VertexBufferObjectID);
		}

		bool ReadFileIntoString(const std::string& Filepath, std::string& OutString)
		{
			std::ifstream File(Filepath);
			if (!File.is_open())
			{
				return false;
			}

			OutString = {};
			std::string Line;
			while (std::getline(File, Line))
			{
				OutString += Line;
				OutString += '\n';
			}

			File.close();

			return true;
		}

		bool CompileShaderSource(GLenum ShaderStage, const std::string& Source, uint32_t& OutShader, char* ErrorMessageBuffer = nullptr, size_t ErrorMessageBufferSizeBytes = 512)
		{
			OutShader = glCreateShader(ShaderStage);

			const char* const SourceCString = Source.c_str();
			glShaderSource(OutShader, 1, &SourceCString, nullptr);
			glCompileShader(OutShader);

			int32_t Success;
			glGetShaderiv(OutShader, GL_COMPILE_STATUS, &Success);

			if (!Success)
			{
				if (ErrorMessageBuffer)
				{
					glGetShaderInfoLog(OutShader, ErrorMessageBufferSizeBytes, nullptr, ErrorMessageBuffer);
				}
				return false;
			}

			return true;
		}

		bool CreatePipeline(const std::string& VertexShaderSourceFilepath, const std::string& FragmentShaderSourceFilepath, Core::Uuid& OutUuid)
		{
			// Read source code file into string buffer
			std::string VertexShaderSourceCode;
			if (!OpenGLRHIInternals::ReadFileIntoString(VertexShaderSourceFilepath, VertexShaderSourceCode))
			{
				return false;
			}

			std::string FragmentShaderSourceCode;
			if (!OpenGLRHIInternals::ReadFileIntoString(FragmentShaderSourceFilepath, FragmentShaderSourceCode))
			{
				return false;
			}

			// Compile shader source code
			static constexpr size_t CompileErrorMessageBufferSizeBytes = 512;
			std::array<char, CompileErrorMessageBufferSizeBytes> CompileErrorMessageBuffer;

			uint32_t VertexShader;
			if (!OpenGLRHIInternals::CompileShaderSource(
				GL_VERTEX_SHADER,
				VertexShaderSourceCode,
				VertexShader,
				CompileErrorMessageBuffer.data(),
				CompileErrorMessageBufferSizeBytes
			))
			{
				CONSOLE_PRINTF("Shader compilation error. File: %s. Message: %s\n", VertexShaderSourceFilepath.c_str(), CompileErrorMessageBuffer.data());
				return false;
			}

			uint32_t FragmentShader;
			if (!OpenGLRHIInternals::CompileShaderSource(
				GL_FRAGMENT_SHADER,
				FragmentShaderSourceCode,
				FragmentShader,
				CompileErrorMessageBuffer.data(),
				CompileErrorMessageBufferSizeBytes
			))
			{
				CONSOLE_PRINTF("Shader compilation error. File: %s. Message: %s\n", FragmentShaderSourceFilepath.c_str(), CompileErrorMessageBuffer.data());
				return false;
			}

			// Create shader program
			OpenGLRHIInternals::OpenGL_GraphicsPipeline Pipeline = {};

			Pipeline.ShaderProgramObject = glCreateProgram();

			glAttachShader(Pipeline.ShaderProgramObject, VertexShader);
			glAttachShader(Pipeline.ShaderProgramObject, FragmentShader);
			glLinkProgram(Pipeline.ShaderProgramObject);

			int32_t Success;
			glGetProgramiv(Pipeline.ShaderProgramObject, GL_LINK_STATUS, &Success);
			if (!Success)
			{
				std::array<char, 512> ProgramInfoLog = {};
				glGetProgramInfoLog(Pipeline.ShaderProgramObject, 512, nullptr, ProgramInfoLog.data());
				CONSOLE_PRINTF("Shader program linking error. Info log: %s\n", ProgramInfoLog.data());
			}

			// Generate uuid and add the new pipeline to the pipeline contianer data structure
			OutUuid.Generate();
			OpenGLRHIInternals::GraphicsPipelines.emplace(OutUuid, Pipeline);

			// Delete shaders
			glDeleteShader(VertexShader);
			glDeleteShader(FragmentShader);

			return true;
		}

		void DestroyGraphicsPipelineResources(const OpenGLRHIInternals::OpenGL_GraphicsPipeline& Pipeline)
		{
			glDeleteProgram(Pipeline.ShaderProgramObject);
		}
	}
}

bool Rendering::RenderHardwareInterface::Initialize(void* const OutputWindowPlatformHandle)
{
	// Does a context already exist for the output window. If so, rendering has already been initialized for the output window
	if (OpenGLRHIInternals::DoesOutputWindowResourcesExist(OutputWindowPlatformHandle))
	{
		return false;
	}

	// Initialize output window, creating a context for the window
	if (!CreateOutputWindowResources(OutputWindowPlatformHandle))
	{
		return false;
	}

	// Set output window, making its context the current context
	if (!SetOutputWindow(OutputWindowPlatformHandle))
	{
		return false;
	}

	// Load the api functions. This only needs to be done once during startup but needs a valid current context
	if (!OpenGLRHIInternals::LoadOpenGLFunctions())
	{
		return false;
	}

	// Print api version
	OpenGLRHIInternals::PrintOpenGLVersion();

	// Create rendering pipelines
	if (!OpenGLRHIInternals::CreatePipeline(
		"Shaders/MeshVertexShader.glsl",
		"Shaders/MeshFragmentShader.glsl",
		OpenGLRHIInternals::MeshGraphicsPipeline
	))
	{
		return false;
	}

	// Remove current context meaning that there is no rendering context set after initialization
	return OpenGLRHIInternals::ClearCurrentContext();
}

bool Rendering::RenderHardwareInterface::CreateOutputWindowResources(void* const OutputWindowPlatformHandle)
{
	OpenGLRHIInternals::OpenGL_OutputWindowResources Resources = {};

	// Create a context for the output window
	Resources.OpenGLContext = OpenGLRHIInternals::CreateContext(OutputWindowPlatformHandle);
	if (!Resources.OpenGLContext)
	{
		return false;
	}

	// Add the resources structure to the output window resource map
	return OpenGLRHIInternals::AddOutputWindowResources(OutputWindowPlatformHandle, Resources);
}

bool Rendering::RenderHardwareInterface::DestroyOutputWindowResources(void* const OutputWindowPlatformHandle)
{
	// Get resources for the output window
	OpenGLRHIInternals::OpenGL_OutputWindowResources Resources;
	if (!OpenGLRHIInternals::GetOutputWindowResources(OutputWindowPlatformHandle, Resources))
	{
		return false;
	}

	OpenGLRHIInternals::FreeOutputWindowResources(OutputWindowPlatformHandle, Resources);

	OpenGLRHIInternals::OutputWindowResources.erase(OutputWindowPlatformHandle);

	return true;
}

bool Rendering::RenderHardwareInterface::SetOutputWindow(void* const OutputWindowPlatformHandle)
{
	// If a null output window platform handle is passed, clear the opengl context
	if (!OutputWindowPlatformHandle)
	{
		return OpenGLRHIInternals::ClearCurrentContext();
	}

	// Make the opengl context associated with the output window platform handle current
	OpenGLRHIInternals::OpenGL_OutputWindowResources Resources;
	if (!OpenGLRHIInternals::GetOutputWindowResources(OutputWindowPlatformHandle, Resources))
	{
		return false;
	}

	return OpenGLRHIInternals::MakeContextCurrent(OutputWindowPlatformHandle, Resources.OpenGLContext);
}

void Rendering::RenderHardwareInterface::SetViewport(const int32_t X, const int32_t Y, const int32_t Width, const int32_t Height)
{
	glViewport(X, Y, Width, Height);
}

void Rendering::RenderHardwareInterface::ClearColorBuffer(const float R, const float G, const float B, const float A)
{
	glClearColor(R, G, B, A);
	glClear(GL_COLOR_BUFFER_BIT);
}

bool Rendering::RenderHardwareInterface::SwapOutputWindowBuffers(void* const OutputWindowPlatformHandle)
{
	return OpenGLRHIInternals::SwapWindowBuffers(OutputWindowPlatformHandle);
}

void Rendering::RenderHardwareInterface::Cleanup()
{
	// Clear current opengl context
	OpenGLRHIInternals::ClearCurrentContext();

	// Cleanup output window resources
	for (const std::pair<void*, OpenGLRHIInternals::OpenGL_OutputWindowResources>& Pair : OpenGLRHIInternals::OutputWindowResources)
	{
		OpenGLRHIInternals::FreeOutputWindowResources(Pair.first, Pair.second);
	}
	OpenGLRHIInternals::OutputWindowResources.clear();

	// Cleanup mesh render objects
	for (const std::pair<Core::Uuid, OpenGLRHIInternals::OpenGL_MeshRenderObject>& Pair : OpenGLRHIInternals::MeshRenderObjects)
	{
		OpenGLRHIInternals::DestroyMeshRenderObjectResources(Pair.second);
	}
	OpenGLRHIInternals::MeshRenderObjects.clear();

	// Cleanup pipelines
	for (const std::pair<Core::Uuid, OpenGLRHIInternals::OpenGL_GraphicsPipeline>& Pair : OpenGLRHIInternals::GraphicsPipelines)
	{
		OpenGLRHIInternals::DestroyGraphicsPipelineResources(Pair.second);
	}
	OpenGLRHIInternals::GraphicsPipelines.clear();

	OpenGLRHIInternals::MeshGraphicsPipeline.Reset();
}

Core::Uuid Rendering::RenderHardwareInterface::CreateMeshRenderObject(const size_t VertexDataSize, const void* VertexData)
{
	// Generate identifier for mesh render object
	Core::Uuid uuid = {};
	uuid.Generate();

	// Generate vertex buffer on GPU and upload data to it
	uint32_t VertexBuffer;
	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, VertexDataSize, VertexData, GL_STATIC_DRAW);

	// Add mesh object to mesh map with unique identifier key
	OpenGLRHIInternals::MeshRenderObjects.emplace(uuid, VertexBuffer);

	// Return identifier
	return uuid;
}

void Rendering::RenderHardwareInterface::DestroyMeshRenderObject(const Core::Uuid& ObjectUuid)
{
	OpenGLRHIInternals::DestroyMeshRenderObjectResources(OpenGLRHIInternals::MeshRenderObjects.at(ObjectUuid));
	OpenGLRHIInternals::MeshRenderObjects.erase(ObjectUuid);
}
