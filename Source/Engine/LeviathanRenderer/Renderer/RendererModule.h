#pragma once

#include "Core/Module.h"

namespace Renderer
{
	enum class BufferUsage : uint8_t
	{
		Stream,
		Static,
		Dynamic,
		MAX
	};

	class RendererModule : public Core::Module
	{
	public:
		RendererModule();

		void* CreateContext(void* WindowPlatformHandle);
		bool MakeContextCurrent(void* WindowPlatformHandle, void* Context);
		bool DeleteContext(void* WindowPlatformHandle, void* Context);
		bool LoadAPI();
		void PrintVersion();
		bool SwapWindowBuffers(void* WindowPlatformHandle);

		void Viewport(int32_t LowerLeftX, int32_t LowerLeftY, int32_t WidthPixels, int32_t HeightPixels);
		void ClearColor(float R, float G, float B, float A);
		void Clear();
		void GenBuffers(size_t NumBuffers, uint32_t* OutBufferIDsStart);
		void BindVertexBuffer(uint32_t BufferID);
		void CopyDataToVertexBuffer(uint32_t BufferID, size_t DataSize, const void* Data, BufferUsage Usage);
	};
}