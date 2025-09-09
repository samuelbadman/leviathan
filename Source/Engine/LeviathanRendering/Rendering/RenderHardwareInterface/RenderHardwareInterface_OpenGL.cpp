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

		bool AddWindowAndContext(void* const WindowPlatformHandle, void* const Context)
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

	// Create a context for the output window
	void* Context = OpenGLRHIInternals::CreateContext(OutputWindowPlatformHandle);

	if (!Context)
	{
		return false;
	}

	OpenGLRHIInternals::AddWindowAndContext(OutputWindowPlatformHandle, Context);

	// Make the output window context the current context
	if (!OpenGLRHIInternals::MakeContextCurrent(OutputWindowPlatformHandle, Context))
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

	return true;
}

bool Rendering::RenderHardwareInterface::ShutdownOutputWindow(void* const OutputWindowPlatformHandle)
{
	if (OpenGLRHIInternals::DoesWindowContextExist(OutputWindowPlatformHandle))
	{
		return OpenGLRHIInternals::DeleteContext(OutputWindowPlatformHandle, OpenGLRHIInternals::GetWindowContext(OutputWindowPlatformHandle));
	}
	return false;
}
