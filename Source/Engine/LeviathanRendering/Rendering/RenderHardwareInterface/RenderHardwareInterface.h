#pragma once

#include "Core/Platform/Uuid.h"

namespace Rendering
{
	namespace RenderHardwareInterface
	{
		bool Initialize(void* const OutputWindowPlatformHandle);
		bool CreateOutputWindowResources(void* const OutputWindowPlatformHandle);
		bool DestroyOutputWindowResources(void* const OutputWindowPlatformHandle);
		bool SetOutputWindow(void* const OutputWindowPlatformHandle);
		bool SwapOutputWindowBuffers(void* const OutputWindowPlatformHandle);
		void Cleanup();

		Core::Uuid AllocateStaticRenderMesh(const size_t VertexDataSize, const void* VertexData);
		void ReleaseStaticRenderMesh(const Core::Uuid& StaticRenderMeshUuid);

		void SetViewport(const int32_t X, const int32_t Y, const int32_t Width, const int32_t Height);
		void ClearColorBuffer(const float R, const float G, const float B, const float A);
	};
}