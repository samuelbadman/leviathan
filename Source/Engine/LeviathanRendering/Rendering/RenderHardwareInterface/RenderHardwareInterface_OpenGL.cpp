#include "RenderHardwareInterface.h"
#include "glad/glad.h"
#include "Core/ConsoleOutput.h"

namespace
{
	namespace OpenGLRHIInternals
	{
		// Mapping of platform window handles to opengl contexts
		std::unordered_map<void*, void*> OutputWindowContexts = {};

		bool DoesWindowContextExist(void* const WindowPlatformHandle)
		{
			return (OpenGLRHIInternals::OutputWindowContexts.find(WindowPlatformHandle) != OpenGLRHIInternals::OutputWindowContexts.end());
		}

		void* GetWindowContext(void* const WindowPlatformHandle)
		{
			return (DoesWindowContextExist(WindowPlatformHandle)) ? OutputWindowContexts.at(WindowPlatformHandle) : nullptr;
		}

		bool AddWindowContext(void* const WindowPlatformHandle, void* const Context)
		{
			if (!DoesWindowContextExist(WindowPlatformHandle))
			{
				OpenGLRHIInternals::OutputWindowContexts.emplace(WindowPlatformHandle, Context);
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

		bool LoadOGLFunctions()
		{
			return gladLoadGL() != 0;
		}

		void PrintOGLVersion()
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
	}
}

bool Rendering::RenderHardwareInterface::Initialize(void* const OutputWindowPlatformHandle)
{
	// Does a context already exist for the output window. If so, rendering has already been initialized for the output window
	if (OpenGLRHIInternals::DoesWindowContextExist(OutputWindowPlatformHandle))
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
	if (!OpenGLRHIInternals::LoadOGLFunctions())
	{
		return false;
	}

	// Debug print api version
	OpenGLRHIInternals::PrintOGLVersion();

	// Remove current context meaning that there is no rendering context set after initialization
	return OpenGLRHIInternals::MakeContextCurrent(OutputWindowPlatformHandle, nullptr);
}

bool Rendering::RenderHardwareInterface::CreateOutputWindowResources(void* const OutputWindowPlatformHandle)
{
	// Create a context for the output window
	if (void* const Context = OpenGLRHIInternals::CreateContext(OutputWindowPlatformHandle))
	{
		return OpenGLRHIInternals::AddWindowContext(OutputWindowPlatformHandle, Context);
	}
	return false;
}

bool Rendering::RenderHardwareInterface::DestroyOutputWindowResources(void* const OutputWindowPlatformHandle)
{
	if (OpenGLRHIInternals::DoesWindowContextExist(OutputWindowPlatformHandle))
	{
		return OpenGLRHIInternals::DeleteContext(OutputWindowPlatformHandle, OpenGLRHIInternals::GetWindowContext(OutputWindowPlatformHandle));
	}
	return false;
}

bool Rendering::RenderHardwareInterface::SetOutputWindow(void* const OutputWindowPlatformHandle)
{
	// Remove rendering context if a null handle has been passed otherwise, make the output window context the current context
	return OpenGLRHIInternals::MakeContextCurrent(OutputWindowPlatformHandle, 
		(OutputWindowPlatformHandle) ? OpenGLRHIInternals::GetWindowContext(OutputWindowPlatformHandle) : nullptr);
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

bool Rendering::RenderHardwareInterface::SwapBuffers(void* const OutputWindowPlatformHandle)
{
	return OpenGLRHIInternals::SwapWindowBuffers(OutputWindowPlatformHandle);
}
