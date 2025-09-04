#pragma once

#include "Core/Module.h"

namespace Renderer
{
	class RendererModule : public Core::Module
	{
	public:
		RendererModule();

		void* CreateContext(void* WindowPlatformHandle);
		bool MakeContextCurrent(void* WindowPlatformHandle, void* Context);
		bool DeleteContext(void* WindowPlatformHandle, void* Context);
		bool LoadAPI();
		void PrintVersion();


	};
}