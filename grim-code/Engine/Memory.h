#pragma once

namespace mem 
{
	struct SizeAlign
	{
		SizeAlign() {}
		SizeAlign(uint32_t size, uint32_t align) :size(size), align(align) {}

		uint32_t size;
		uint32_t align;
	};

	class LinearAllocator
	{
		enum 
		{
			kDefaultMemoryAlignment = 4,
			kMaxAllocatorNameLength = 64
		};
	public:
		LinearAllocator(void* memory, uint32_t memorySize, const char* name);
		
		void* Allocate(const SizeAlign& sizeAlign);
		void* Allocate(const uint32_t size, const uint32_t align = 4);
		void Clear();
		void Restore(void* restorePoint, bool zeroFreed = false);

	private:
		uint64_t m_memStart;
		uint64_t m_memEnd;
		uint64_t m_memCurrent;

		char m_name[kMaxAllocatorNameLength]; // TODO better const name length
	};

	void Init();
	void Shutdown();

	LinearAllocator* GetScratchpad();
}