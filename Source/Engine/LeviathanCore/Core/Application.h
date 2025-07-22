#pragma once

#include "PlatformEntryPointPrototype.h"

#define IMPLEMENT(ApplicationType)\
	PLATFORM_ENTRY_POINT_PROTOTYPE\
	{\
		auto* Application = new ApplicationType;\
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