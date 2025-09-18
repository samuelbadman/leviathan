#pragma once

namespace Rendering
{
	namespace RenderHardwareInterface
	{
		struct Context;

		/*
			Resource management -- create, read, update, delete
			Frame management -- When to start a frame, what swapchain image is being used etc
			Command recording -- essentially one big class that records all commands using the same API
		*/

		bool Initialize(void* const InitWindowPlatformHandle);

		Context* NewContext(void* const WindowPlatformHandle);
		bool DeleteContext(Context* const pContext);
		bool MakeContextCurrent(Context* const pContext);

		bool SwapWindowBuffers(void* const WindowPlatformHandle);

		void SetViewport(const int32_t X, const int32_t Y, const int32_t Width, const int32_t Height);
		void ClearColorBuffer(const float R, const float G, const float B, const float A);
	};
}