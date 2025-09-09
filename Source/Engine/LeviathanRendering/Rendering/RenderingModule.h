#pragma once

#include "Core/Module.h"

namespace Rendering
{
	class RenderingModule : public Core::Module
	{
	public:
		bool Initialize(void* const OutputWindowPlatformHandle);
		bool ShutdownOutputWindow(void* const OutputWindowPlatformHandle);
	};
}