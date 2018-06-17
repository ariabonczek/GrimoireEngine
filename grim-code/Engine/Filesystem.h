#pragma once

namespace grimFile
{

	void Init();
	char* LoadTextFile(const char* filename, char* buffer = nullptr, uint32_t bufferSize = 0);
}