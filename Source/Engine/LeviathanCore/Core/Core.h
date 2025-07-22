#pragma once

#define PlatformEntry int main()

#define Implement(ApplicationType)\
	PlatformEntry\
	{\
		auto* Application = new ApplicationType;\
		return 0;\
	}
