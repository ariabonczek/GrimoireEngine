#include "Assert.h"

#include <Windows.h>

void WriteLogMessage(char* format, ...)
{
	char cbuffer[1024];

	va_list args;
	va_start(args, format);
	
	int last = _vsnprintf(cbuffer, sizeof cbuffer, format, args);
	cbuffer[last++] = '\n';
	cbuffer[last++] = '\0';
	
	printf(cbuffer);
	OutputDebugString(cbuffer);

	va_end(args);
}