#pragma once

#include <intrin.h>
#include <stdio.h>
#include <stdarg.h>

#if defined(_DEBUG)

#define GRIM_BREAK() __debugbreak()
#define GRIM_LOG(message, ...) { WriteLogMessage(message, ##__VA_ARGS__); }
#define GRIM_ASSERT(condition, message, ...) { if(!(condition)) { WriteLogMessage(message, ##__VA_ARGS__); GRIM_BREAK(); }}
#define GRIM_WARNIFFALSE(condition, message, ...) { if(!(condition)) { WriteLogMessage(message, ##__VA_ARGS__); }}

#else

#define GRIM_BREAK()
#define GRIM_LOG(message, ...)
#define GRIM_ASSERT(condition, message, ...)
#define GRIM_WARNIFFALSE(condition, message, ...)

#endif

void WriteLogMessage(char* format, ...);
