#include "Precompiled.h"

#include <Engine/Memory.h>

#if PLATFORM_PC
#include <Engine/WinMem.h>
#endif

#include <Engine/EngineGlobals.h>

static const uint32_t kScratchMemoryPerThreadInBytes = 20 * 1024 * 1024;

mem::LinearAllocator* mem::GetScratchpad()
{
	thread_local void* scratch = malloc(kScratchMemoryPerThreadInBytes);
	thread_local LinearAllocator allocator(scratch, kScratchMemoryPerThreadInBytes, "Thread Whiteboard Memory");

	return &allocator;
}

#if GRIM_MEMORY_TRACE
	struct BlockStats
	{
		enum
		{
			kMaxFileNameLength = 32
		};
		uint64_t address;
		uint64_t time;
		LinearAllocator* allocator;
		uint32_t size;

		char fileName[kMaxFileNameLength];
		uint16_t lineNumber;
	};
	static const uint32_t kMaxBlocks = 1024;
	static BlockStats* s_stats;
	static uint32_t s_numBlocks;
#endif
	
void mem::Init()
{
	//s_pageSize = plat::GetVirtualPageSize();
	//
	//// Round ram size up to nearest page
	//uint32_t overload = g_engineGlobals.cpuRamSize % s_pageSize;
	//uint32_t delta = overload > 0 ? s_pageSize - overload : 0;
	//
	//s_cpuRamInBytes = g_engineGlobals.cpuRamSize + delta;
	//s_numPages = s_cpuRamInBytes / s_pageSize;
	//
	//s_memoryStart = plat::ReservePages(s_numPages, s_pageSize);
	//
	//memset(s_memoryStart, 0xab, s_numPages * s_pageSize);
	//
	//s_memoryCurrent = s_memoryStart;
	//s_memoryEnd = static_cast<unsigned*>(s_memoryStart) + s_cpuRamInBytes;
}

void mem::Shutdown()
{
	//plat::FreeVirtualMemory(s_memoryStart, s_numPages, s_pageSize);
}

// Utility ------------------------------------------------------------------------------------

static bool FoundInRegion(void* ptr, void* regionStart, uint64_t regionSize)
{
	return ptr > regionStart && ptr < static_cast<unsigned*>(regionStart) + regionSize;
}	

// Allocator ----------------------------------------------------------------------------------

mem::LinearAllocator::LinearAllocator(void* memory, uint32_t memorySize, const char* name)
{
	GRIM_ASSERT(memory != nullptr && memorySize > 0, "Invalid memory given to linear allocator %s", name);

	m_memStart = (uint64_t)memory;
	m_memCurrent = m_memStart;
	m_memEnd = m_memStart + memorySize;

	snprintf(m_name, kMaxAllocatorNameLength, "%s", name);
}

void* mem::LinearAllocator::Allocate(const SizeAlign& sizeAlign)
{
	return Allocate(sizeAlign.size, sizeAlign.align);
}

void* mem::LinearAllocator::Allocate(const uint32_t size, const uint32_t alignment)
{
	m_memCurrent = m_memCurrent + (uint64_t)((alignment - 1) & ~(alignment - 1));

	GRIM_ASSERT(m_memCurrent + size < m_memEnd, "LinearAllocator %s has run out of space trying to allocate %u bytes", m_name, size);
	
	// round memcurrent up to the nearest alignment boundary
	void* ret = (void*)m_memCurrent;
	m_memCurrent += size;
	return ret;
}

void  mem::LinearAllocator::Clear()
{
	m_memCurrent = m_memStart;
}

void  mem::LinearAllocator::Restore(void* restorePoint, bool zeroFreed)
{
	GRIM_ASSERT(FoundInRegion(restorePoint, (void*)m_memStart, m_memEnd - m_memStart), "Tried to restore LinearAllocator %s to address %p which is not in the memory region", m_name, restorePoint);
	GRIM_ASSERT((uint64_t)restorePoint <= m_memCurrent, "Tried to restore LinearAllocator %s to address %p which is after the current address %p", m_name, restorePoint, (void*)m_memCurrent);

	if (zeroFreed)
		memset(restorePoint, 0, m_memCurrent - (uint64_t)restorePoint);

	m_memCurrent = (uint64_t)restorePoint;
}
