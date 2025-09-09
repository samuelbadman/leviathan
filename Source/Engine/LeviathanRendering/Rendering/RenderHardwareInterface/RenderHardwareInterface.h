#pragma once

namespace Rendering
{
	namespace RenderHardwareInterface
	{
		bool Initialize(void* const OutputWindowPlatformHandle);
		bool CreateOutputWindowResources(void* const OutputWindowPlatformHandle);
		bool DestroyOutputWindowResources(void* const OutputWindowPlatformHandle);
		bool SetOutputWindow(void* const OutputWindowPlatformHandle);
		bool SwapBuffers(void* const OutputWindowPlatformHandle);

		void SetViewport(const int32_t X, const int32_t Y, const int32_t Width, const int32_t Height);
		void ClearColorBuffer(const float R, const float G, const float B, const float A);
	};
}