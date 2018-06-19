#include "Precompiled.h"

#include <Engine/Memory.h>

#if PLATFORM_PC
#include <Engine/WinMem.h>
#endif

#include <Engine/EngineGlobals.h>

#include <Shared/Math/Math.h>

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

static bool FoundInRegion(void* ptr, void* regionStart, size_t regionSize)
{
	return ptr > regionStart && ptr < static_cast<unsigned*>(regionStart) + regionSize;
}

static size_t alignSize(size_t originalSize, size_t align)
{
	return originalSize + +(align - 1) & ~(align - 1);
}

static void* alignMemory(void* memory, size_t align)
{
	GRIM_ASSERT(memory != nullptr && align > 0, "");

	return (void*)((size_t)memory + (align - 1) & ~(align - 1));
}

// Allocator ----------------------------------------------------------------------------------

mem::LinearAllocator::LinearAllocator(void* memory, size_t memorySize, const char* name)
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

void* mem::LinearAllocator::Allocate(const size_t size, const size_t alignment)
{
	m_memCurrent = m_memCurrent + (alignment - 1) & ~(alignment - 1);

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
	GRIM_ASSERT(FoundInRegion(restorePoint, (void*)m_memStart,(uint32_t)( m_memEnd - m_memStart)), "Tried to restore LinearAllocator %s to address %p which is not in the memory region", m_name, restorePoint);
	GRIM_ASSERT((uint64_t)restorePoint <= m_memCurrent, "Tried to restore LinearAllocator %s to address %p which is after the current address %p", m_name, restorePoint, (void*)m_memCurrent);

	if (zeroFreed)
		memset(restorePoint, 0, m_memCurrent - (uint64_t)restorePoint);

	m_memCurrent = (uint64_t)restorePoint;
}

mem::PoolAllocator::PoolAllocator(void* memory, size_t memorySize, size_t poolElementSize, const char* name) :
	m_elementSize((uint32_t)alignSize(max(poolElementSize, (size_t)sizeof(void*)), sizeof(void*))),
	m_poolStart((uint64_t)memory),
	m_nextFree(m_poolStart),
	m_capacity((uint32_t)(memorySize / poolElementSize))
{
	GRIM_ASSERT(memory != nullptr && memorySize > 0, "Invalid memory given to pool allocator %s", name);
	GRIM_ASSERT(poolElementSize > 0 && poolElementSize <= memorySize, "Invalid pool element size given to pool allocator", name);

	for (int i = 0; i < (int)m_capacity - 1; ++i)
	{
		void** current = (void**)m_poolStart + i * m_elementSize;
		void** next = (void**)current + m_elementSize;

		*current = *next;
	}

	void** last = (void**)m_poolStart + (m_capacity - 1) * m_elementSize;
	*last = nullptr;

	snprintf(m_name, kMaxAllocatorNameLength, "%s", name);
}

void* mem::PoolAllocator::Allocate()
{
	GRIM_ASSERT((void*)m_nextFree != nullptr, "");

	void* ret = (void*)m_nextFree;
	m_nextFree = (size_t)*(void**)m_nextFree;

	return ret;
}

void mem::PoolAllocator::Free(void* ptr)
{
	*(void**)ptr = (void*)m_nextFree;
	m_nextFree = (size_t)ptr;
}