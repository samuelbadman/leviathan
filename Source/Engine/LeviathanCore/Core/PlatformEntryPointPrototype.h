#pragma once

#ifdef PLATFORM_WINDOWS
#define PLATFORM_ENTRY_POINT_PROTOTYPE int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int ncmdShow)
#else
#define PLATFORM_ENTRY_POINT_PROTOTYPE // Building without specifying a build platform is not supported
#endif // PLATFORM_WINDOWS