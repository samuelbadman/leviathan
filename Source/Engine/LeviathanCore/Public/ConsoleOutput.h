#pragma once

#define CONSOLE_PRINTF(Format, ...) printf(Format, __VA_ARGS__)

#ifdef CONFIG_DEBUG
#define DEBUG_CONSOLE_PRINTF(Format, ...) CONSOLE_PRINTF(Format, __VA_ARGS__)
#else
#define DEBUG_CONSOLE_PRINTF(Format, ...)
#endif // CONFIG_DEBUG