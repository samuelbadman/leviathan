#pragma once

#ifdef PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Xinput.h>
#include <Rpc.h> // Uuids
#endif // PLATFORM_WINDOWS

#include <memory>
#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <algorithm>