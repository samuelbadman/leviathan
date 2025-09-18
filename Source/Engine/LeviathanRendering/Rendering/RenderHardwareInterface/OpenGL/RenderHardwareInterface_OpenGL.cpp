#include "Rendering/RenderHardwareInterface/RenderHardwareInterface.h"
#include "glad/glad.h"
#include "Core/ConsoleOutput.h"

namespace
{
	namespace GL_RHI
	{
		struct GL_Context
		{
			void* Context = nullptr;
			void* PlatformWindowHandle = nullptr;
		};

		bool LoadGLFunctions()
		{
			return gladLoadGL() != 0;
		}

		void PrintGLVersion()
		{
			CONSOLE_PRINTF("OpenGL version and driver version: %s\n", glGetString(GL_VERSION));
		}

		//bool ReadShaderSourceFromDisk(const std::string& SourceFilepath, std::string& OutSourceString)
		//{
		//	std::ifstream File(SourceFilepath);
		//	if (!File.is_open())
		//	{
		//		return false;
		//	}

		//	OutSourceString = {};
		//	std::string Line;
		//	while (std::getline(File, Line))
		//	{
		//		OutSourceString += Line;
		//		OutSourceString += '\n';
		//	}

		//	File.close();

		//	return true;
		//}

		//bool CompileShaderSourceString(GLenum ShaderStage, const std::string& Source, uint32_t& OutShader, char* ErrorMessageBuffer = nullptr, size_t ErrorMessageBufferSizeBytes = 512)
		//{
		//	OutShader = glCreateShader(ShaderStage);

		//	const char* const SourceCString = Source.c_str();
		//	glShaderSource(OutShader, 1, &SourceCString, nullptr);
		//	glCompileShader(OutShader);

		//	int32_t Success;
		//	glGetShaderiv(OutShader, GL_COMPILE_STATUS, &Success);

		//	if (!Success)
		//	{
		//		if (ErrorMessageBuffer)
		//		{
		//			glGetShaderInfoLog(OutShader, ErrorMessageBufferSizeBytes, nullptr, ErrorMessageBuffer);
		//		}
		//		return false;
		//	}

		//	return true;
		//}
	}
}

bool Rendering::RenderHardwareInterface::Initialize(void* const InitWindowPlatformHandle)
{
	// Create init context and make it current. A current gl context is needed to load api functions from os/driver
	if (Rendering::RenderHardwareInterface::Context* InitContext = Rendering::RenderHardwareInterface::NewContext(InitWindowPlatformHandle))
	{
		if (Rendering::RenderHardwareInterface::MakeContextCurrent(InitContext))
		{
			// Load gl api
			if (!GL_RHI::LoadGLFunctions())
			{
				return false;
			}

			GL_RHI::PrintGLVersion();

			// Clear current context
			if (!Rendering::RenderHardwareInterface::MakeContextCurrent(nullptr))
			{
				return false;
			}

			// Delete init context
			if (!Rendering::RenderHardwareInterface::DeleteContext(InitContext))
			{
				return false;
			}

			return true;
		}
	}

	return false;
}

Rendering::RenderHardwareInterface::Context* Rendering::RenderHardwareInterface::NewContext(void* const WindowPlatformHandle)
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

	// Create gl context
	GL_RHI::GL_Context* GLContext = new GL_RHI::GL_Context();
	GLContext->Context = static_cast<void*>(wglCreateContext(WindowHandleToDeviceContext));
	GLContext->PlatformWindowHandle = WindowPlatformHandle;

	return reinterpret_cast<Rendering::RenderHardwareInterface::Context*>(GLContext);
#else
	return nullptr;
#endif // PLATFORM_WINDOWS
}

bool Rendering::RenderHardwareInterface::DeleteContext(Rendering::RenderHardwareInterface::Context* const pContext)
{
#ifdef PLATFORM_WINDOWS

	GL_RHI::GL_Context* const GLContext = reinterpret_cast<GL_RHI::GL_Context* const>(pContext);

	if (!GLContext)
	{
		return false;
	}

	if (wglDeleteContext(static_cast<HGLRC>(GLContext->Context)) != TRUE)
	{
		return false;
	}

	delete GLContext;
	return true;
#else
	return false;
#endif // PLATFORM_WINDOWS
}

bool Rendering::RenderHardwareInterface::MakeContextCurrent(Context* const pContext)
{
#ifdef PLATFORM_WINDOWS
	// If null context is passed, clear the current gl context
	if (!pContext)
	{
		return wglMakeCurrent(NULL, NULL) == TRUE;
	}

	GL_RHI::GL_Context* const GLContext = reinterpret_cast<GL_RHI::GL_Context* const>(pContext);

	if (!GLContext)
	{
		return false;
	}

	return wglMakeCurrent(GetDC(static_cast<HWND>(GLContext->PlatformWindowHandle)), static_cast<HGLRC>(GLContext->Context)) == TRUE;
#else
	return false;
#endif // PLATFORM_WINDOWS
}

bool Rendering::RenderHardwareInterface::SwapWindowBuffers(void* const WindowPlatformHandle)
{
#ifdef PLATFORM_WINDOWS
	return SwapBuffers(GetDC(static_cast<HWND>(WindowPlatformHandle))) == TRUE;
#else
	return false;
#endif // PLATFORM_WINDOWS
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