#pragma once

#include "Core/Module.h"

namespace Renderer
{
	class RendererModule : public Core::Module
	{
	public:
		void* CreateContext(void* WindowPlatformHandle);
		bool MakeContextCurrent(void* WindowPlatformHandle, void* Context);
		bool DeleteContext(void* WindowPlatformHandle, void* Context);

		// Requires a valid current context to be set
		bool LoadAPI();

		void PrintVersion();
		bool SwapWindowBuffers(void* WindowPlatformHandle);

	private:

	};
}