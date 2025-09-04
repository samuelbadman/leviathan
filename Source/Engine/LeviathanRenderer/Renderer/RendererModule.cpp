#include "RendererModule.h"
#include "Core/ConsoleOutput.h"
#include "glad/glad.h"

namespace
{
	namespace RendererInternals
	{
		GLenum TranslateBufferUsage(const Renderer::BufferUsage Usage)
		{
			switch (Usage)
			{
			case Renderer::BufferUsage::Stream: return GL_STREAM_DRAW;
			case Renderer::BufferUsage::Static: return GL_STATIC_DRAW;
			case Renderer::BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
			default: return GL_INVALID_ENUM;
			}
		}
	}
}

Renderer::RendererModule::RendererModule()
{
	CONSOLE_PRINTF("Hello renderer module.\n");
}

void* Renderer::RendererModule::CreateContext(void* WindowPlatformHandle)
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

bool Renderer::RendererModule::MakeContextCurrent(void* WindowPlatformHandle, void* Context)
{
#ifdef PLATFORM_WINDOWS
	return wglMakeCurrent(GetDC(static_cast<HWND>(WindowPlatformHandle)), static_cast<HGLRC>(Context)) == TRUE;
#else
	return false;
#endif // PLATFORM_WINDOWS
}

bool Renderer::RendererModule::DeleteContext(void* WindowPlatformHandle, void* Context)
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

bool Renderer::RendererModule::LoadAPI()
{
	return gladLoadGL() != 0;
}

void Renderer::RendererModule::PrintVersion()
{
	CONSOLE_PRINTF("OpenGL version and driver version: %s\n", glGetString(GL_VERSION));
}

bool Renderer::RendererModule::SwapWindowBuffers(void* WindowPlatformHandle)
{
#ifdef PLATFORM_WINDOWS
	return SwapBuffers(GetDC(static_cast<HWND>(WindowPlatformHandle))) == TRUE;
#else
#endif // PLATFORM_WINDOWS
}

void Renderer::RendererModule::Viewport(int32_t LowerLeftX, int32_t LowerLeftY, int32_t WidthPixels, int32_t HeightPixels)
{
	glViewport(LowerLeftX, LowerLeftY, WidthPixels, HeightPixels);
}

void Renderer::RendererModule::ClearColor(float R, float G, float B, float A)
{
	glClearColor(R, G, B, A);
}

void Renderer::RendererModule::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::RendererModule::GenBuffers(size_t NumBuffers, uint32_t* OutBufferIDsStart)
{
	glGenBuffers(NumBuffers, OutBufferIDsStart);
}

void Renderer::RendererModule::BindVertexBuffer(uint32_t BufferID)
{
	glBindBuffer(GL_ARRAY_BUFFER, BufferID);
}

void Renderer::RendererModule::CopyDataToVertexBuffer(uint32_t BufferID, size_t DataSize, const void* Data, BufferUsage Usage)
{
	glBufferData(GL_ARRAY_BUFFER, DataSize, Data, RendererInternals::TranslateBufferUsage(Usage));
}
