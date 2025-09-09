#pragma once

namespace Rendering
{
	namespace RenderHardwareInterface
	{
		bool Initialize(void* const OutputWindowPlatformHandle);
		bool ShutdownOutputWindow(void* const OutputWindowPlatformHandle);
	};
}