#include "Precompiled.h"

#include <Engine/Filesystem.h>

namespace grimFile {

	void Init()
	{

	}

	char* LoadTextFile(const char* filename, char* buffer, uint32_t bufferSize)
	{
		FILE* file = fopen(filename, "r");
		GRIM_ASSERT(file, "Failed to load text file: %s", filename);

		fseek(file, 0, SEEK_END);
		const unsigned long size = ftell(file);
		fseek(file, 0, SEEK_SET);

		if (!buffer)
		{
			bufferSize = size + 1; // +1 for null character
			buffer = static_cast<char*>(malloc(bufferSize));
		}

		GRIM_ASSERT(size < bufferSize, "Text file is larger than the buffer, try increasing the size of the buffer");
		memset(buffer, 0, bufferSize);
		const size_t read = fread(buffer, sizeof(char), size, file);

		buffer[read] = '\0';

		fclose(file);

		return buffer;
	}
}