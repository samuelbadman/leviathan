#pragma once

#define PRINT(Format, ...) printf(Format, __VA_ARGS__)

#ifdef CONFIG_DEBUG
#define DEBUG_PRINT(Format, ...) PRINT(Format, __VA_ARGS__)
#else
#define DEBUG_PRINT(Format, Args)
#endif // CONFIG_DEBUG