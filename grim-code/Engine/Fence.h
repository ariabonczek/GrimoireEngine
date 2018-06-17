#pragma once

#if PLATFORM_PC
#include <Engine/DXFence.h>
#endif

namespace gfx
{
	class CommandContext;

	struct Fence
	{
#if PLATFORM_PC
		plat::DXFence platformFence;
#endif
	};

	void InitFence(Fence& fence);
	void DestroyFence(Fence& fence);
	void TriggerFence(gfx::Fence& fence, const uint64_t value, const CommandQueueIndex queue);
	void CPUWaitOnFence(gfx::Fence& fence, const uint64_t value);
	void GPUStallOnFence(gfx::Fence& fence, const uint64_t value);
}