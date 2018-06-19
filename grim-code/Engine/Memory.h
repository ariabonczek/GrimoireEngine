#pragma once

#include <Shared/Core/SizeAlign.h>

#define kMaxAllocatorNameLength 64

namespace mem
{
	class LinearAllocator
	{
		enum
		{
			kDefaultMemoryAlignment = 4,
		};
	public:
		LinearAllocator(void* memory, size_t memorySize, const char* name);

		void* Allocate(const SizeAlign& sizeAlign);
		void* Allocate(const size_t size, const size_t align = 4);
		void Clear();
		void Restore(void* restorePoint, bool zeroFreed = false);

	private:
		size_t m_memStart;
		size_t m_memEnd;
		size_t m_memCurrent;

		char m_name[kMaxAllocatorNameLength]; // TODO better const name length
	};

	// Single-linked free list stores the "next" pointer inside of the free memory itself
	// Therefore, an element must be at least sizeof(void*)
	// Pool elements are always aligned to sizeof(void*)
	class PoolAllocator
	{
	public:
		PoolAllocator(void* memory, size_t memorySize, size_t poolElementSize, const char* name);

		void* Allocate();
		void Free(void*);

	private:
		size_t m_poolStart;
		size_t m_nextFree;
		uint32_t m_capacity;
		uint32_t m_elementSize;

		char m_name[kMaxAllocatorNameLength]; // TODO better const name length
	};

	void Init();
	void Shutdown();

	LinearAllocator* GetScratchpad();
}