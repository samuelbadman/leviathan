#include "Rendering/RenderHardwareInterface/RenderHardwareInterface.h"
#include "glad/glad.h"
#include "Core/ConsoleOutput.h"

namespace
{
	namespace OpenGLRHIInternals
	{
		struct OpenGLOutputWindowResources
		{
			void* OpenGLContext = nullptr;
		};

		struct OpenGLRenderingPipeline
		{
			uint32_t ShaderProgramObject = 0;

			OpenGLRenderingPipeline() = default;
			OpenGLRenderingPipeline(const uint32_t InShaderProgramObjectID)
				: ShaderProgramObject(InShaderProgramObjectID)
			{
			}
		};

		struct OpenGLStaticRenderMesh
		{
			uint32_t VertexBufferObjectID = 0;

			OpenGLStaticRenderMesh() = default;
			OpenGLStaticRenderMesh(const uint32_t InVertexBufferObjectID)
				: VertexBufferObjectID(InVertexBufferObjectID)
			{
			}
		};

		// Mapping of platform window handles to output window resource structures. The window platform handle is being used as the handle/identifier to its rendering resources
		std::unordered_map<void*, OpenGLOutputWindowResources> OutputWindowResources = {};

		// Mapping of uuid identifers to static render mesh objects
		std::unordered_map<Core::Uuid, OpenGLStaticRenderMesh> AllocatedStaticRenderMeshes = {};

		bool DoesOutputWindowResourcesExist(void* const WindowPlatformHandle)
		{
			return (OpenGLRHIInternals::OutputWindowResources.find(WindowPlatformHandle) != OpenGLRHIInternals::OutputWindowResources.end());
		}

		bool GetOutputWindowResources(void* const WindowPlatformHandle, OpenGLRHIInternals::OpenGLOutputWindowResources& Resources)
		{
			if (DoesOutputWindowResourcesExist(WindowPlatformHandle))
			{
				Resources = OutputWindowResources.at(WindowPlatformHandle);
				return true;
			}
			Resources = {};
			return false;
		}

		bool AddOutputWindowResources(void* const WindowPlatformHandle, const OpenGLRHIInternals::OpenGLOutputWindowResources& Resources)
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

		bool FreeOutputWindowResources(void* const OutputWindowPlatformHandle, const OpenGLRHIInternals::OpenGLOutputWindowResources& Resources)
		{
			// Delete rendering context for the output window
			if (!OpenGLRHIInternals::DeleteContext(OutputWindowPlatformHandle, Resources.OpenGLContext))
			{
				return false;
			}
			return true;
		}

		void ReleaseRenderMeshResources(const OpenGLRHIInternals::OpenGLStaticRenderMesh& RenderMesh)
		{
			glDeleteBuffers(1, &RenderMesh.VertexBufferObjectID);
		}

		enum class RenderingPipelines : uint8_t
		{
			StaticMesh,
			MAX
		};

		Core::Uuid CreatePipeline(const RenderingPipelines Pipeline)
		{
			// Get filepaths to pipeline api shader source code
			std::string VertexShaderSourceFilePath;
			switch (Pipeline)
			{
			case RenderingPipelines::StaticMesh:
				VertexShaderSourceFilePath = "Shaders/StaticMeshVertexShader.glsl";
				break;

			default:
				return Core::Uuid();
			}

			// Read shader source file into string buffer
			std::ifstream VertexShaderSourceFile(VertexShaderSourceFilePath);
			if (!VertexShaderSourceFile.is_open())
			{
				return Core::Uuid();
			}

			std::string VertexShaderSourceString;
			std::string SourceLine;
			while (std::getline(VertexShaderSourceFile, SourceLine))
			{
				VertexShaderSourceString += SourceLine;
				VertexShaderSourceString += '\n';
			}

			VertexShaderSourceFile.close();

			CONSOLE_PRINTF("Vertex Shader Source String:\n%s", VertexShaderSourceString.c_str());

			// Compile shaders
			uint32_t VertexShader;
			VertexShader = glCreateShader(GL_VERTEX_SHADER);

			const char* const C_VertexShaderSourceString = VertexShaderSourceString.c_str();
			glShaderSource(VertexShader, 1, &C_VertexShaderSourceString, nullptr);
			glCompileShader(VertexShader);

			int32_t Success;
			glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Success);

			if (!Success)
			{
				char CompilerMessage[512];
				glGetShaderInfoLog(VertexShader, 512, nullptr, CompilerMessage);

				CONSOLE_PRINTF("Compiler message: %s", CompilerMessage);
			}

			return Core::Uuid();
		}

		void ReleasePipeline(const Core::Uuid& PipelineUuid)
		{
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

	// Debug print api version
	OpenGLRHIInternals::PrintOpenGLVersion();

	// Create rendering pipelines
	OpenGLRHIInternals::CreatePipeline(OpenGLRHIInternals::RenderingPipelines::StaticMesh);

	// Remove current context meaning that there is no rendering context set after initialization
	return OpenGLRHIInternals::ClearCurrentContext();
}

bool Rendering::RenderHardwareInterface::CreateOutputWindowResources(void* const OutputWindowPlatformHandle)
{
	OpenGLRHIInternals::OpenGLOutputWindowResources Resources = {};

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
	OpenGLRHIInternals::OpenGLOutputWindowResources Resources;
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
	OpenGLRHIInternals::OpenGLOutputWindowResources Resources;
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
	for (const std::pair<void*, OpenGLRHIInternals::OpenGLOutputWindowResources>& Pair : OpenGLRHIInternals::OutputWindowResources)
	{
		OpenGLRHIInternals::FreeOutputWindowResources(Pair.first, Pair.second);
	}
	OpenGLRHIInternals::OutputWindowResources.clear();

	// Cleanup render mesh objects
	for (const std::pair<Core::Uuid, OpenGLRHIInternals::OpenGLStaticRenderMesh>& Pair : OpenGLRHIInternals::AllocatedStaticRenderMeshes)
	{
		OpenGLRHIInternals::ReleaseRenderMeshResources(Pair.second);
	}
	OpenGLRHIInternals::AllocatedStaticRenderMeshes.clear();
}

Core::Uuid Rendering::RenderHardwareInterface::AllocateStaticRenderMesh(const size_t VertexDataSize, const void* VertexData)
{
	// Generate identifier for render mesh object
	Core::Uuid uuid = {};
	uuid.Generate();

	// Generate vertex buffer on GPU and upload data to it
	uint32_t VertexBuffer;
	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, VertexDataSize, VertexData, GL_STATIC_DRAW);

	// Add render mesh to render mesh map with unique identifier key
	OpenGLRHIInternals::AllocatedStaticRenderMeshes.emplace(uuid, VertexBuffer);

	// Return identifier
	return uuid;
}

void Rendering::RenderHardwareInterface::ReleaseStaticRenderMesh(const Core::Uuid& StaticRenderMeshUuid)
{
	OpenGLRHIInternals::ReleaseRenderMeshResources(OpenGLRHIInternals::AllocatedStaticRenderMeshes.at(StaticRenderMeshUuid));
	OpenGLRHIInternals::AllocatedStaticRenderMeshes.erase(StaticRenderMeshUuid);
}
