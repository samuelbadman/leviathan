#include "RenderHardwareInterface.h"
#include "glad/glad.h"
#include "Core/ConsoleOutput.h"

namespace
{
	namespace OpenGLRHIInternals
	{
		struct OutputWindowResources
		{
			void* OpenGLContext = nullptr;
		};

		// Mapping of platform window handles to output window resource structures. The window platform handle is being used as the handle/identifier to its rendering resources
		std::unordered_map<void*, OutputWindowResources> OutputWindowResourceMap = {};

		bool DoesOutputWindowResourcesExist(void* const WindowPlatformHandle)
		{
			return (OpenGLRHIInternals::OutputWindowResourceMap.find(WindowPlatformHandle) != OpenGLRHIInternals::OutputWindowResourceMap.end());
		}

		bool GetOutputWindowResources(void* const WindowPlatformHandle, OpenGLRHIInternals::OutputWindowResources& Resources)
		{
			if (DoesOutputWindowResourcesExist(WindowPlatformHandle))
			{
				Resources = OutputWindowResourceMap.at(WindowPlatformHandle);
				return true;
			}
			Resources = {};
			return false;
		}

		bool AddOutputWindowResources(void* const WindowPlatformHandle, const OpenGLRHIInternals::OutputWindowResources& Resources)
		{
			if (!DoesOutputWindowResourcesExist(WindowPlatformHandle))
			{
				OpenGLRHIInternals::OutputWindowResourceMap.emplace(WindowPlatformHandle, Resources);
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

		bool ClearCurrentContext()
		{
#ifdef PLATFORM_WINDOWS
			return OpenGLRHIInternals::MakeContextCurrent(nullptr, nullptr);
#else
			return false;
#endif // PLATFORM_WINDOWS
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
	if (!OpenGLRHIInternals::LoadOGLFunctions())
	{
		return false;
	}

	// Debug print api version
	OpenGLRHIInternals::PrintOGLVersion();

	// Remove current context meaning that there is no rendering context set after initialization
	return OpenGLRHIInternals::ClearCurrentContext();
}

bool Rendering::RenderHardwareInterface::CreateOutputWindowResources(void* const OutputWindowPlatformHandle)
{
	OpenGLRHIInternals::OutputWindowResources Resources = {};

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
	OpenGLRHIInternals::OutputWindowResources Resources;
	if (!OpenGLRHIInternals::GetOutputWindowResources(OutputWindowPlatformHandle, Resources))
	{
		return false;
	}

	// Delete rendering context for the output window
	if (!OpenGLRHIInternals::DeleteContext(OutputWindowPlatformHandle, Resources.OpenGLContext))
	{
		return false;
	}

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
	OpenGLRHIInternals::OutputWindowResources Resources;
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
	// Cleanup output window resources
	for (const std::pair<void*, OpenGLRHIInternals::OutputWindowResources>& Pair : OpenGLRHIInternals::OutputWindowResourceMap)
	{
		DestroyOutputWindowResources(Pair.first);
	}
	OpenGLRHIInternals::OutputWindowResourceMap.clear();
}
