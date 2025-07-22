#pragma once

#include "PlatformEntryPointPrototype.h"
#include "Core.h"

#define IMPLEMENT(ApplicationType)\
	PLATFORM_ENTRY_POINT_PROTOTYPE\
	{\
		Core::BeginApplication(new ApplicationType);\
		return 0;\
	}

namespace Core
{
	class Application
	{
	public:
		virtual ~Application() {};
	};
}