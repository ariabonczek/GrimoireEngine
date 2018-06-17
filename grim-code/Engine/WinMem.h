#pragma once

typedef unsigned int uint32_t;

#include <windows.h>

namespace plat 
{

	inline uint32_t GetVirtualPageSize()
	{
		SYSTEM_INFO info;
		GetSystemInfo(&info);

		return info.dwPageSize;
	}

	inline void* ReservePages(uint32_t numPages, uint32_t pageSize)
	{
		void* mem = VirtualAlloc(NULL, numPages * pageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		GRIM_ASSERT(mem, "Failed to reserve %u pages of virtual memory of size %u, total %u bytes", numPages, pageSize, numPages*pageSize);
		return mem;
	}

	inline void FreeVirtualMemory(void* mem, uint32_t numPages, uint32_t pageSize)
	{
		VirtualFree(mem, numPages * pageSize, MEM_RELEASE);
	}
}