#include "Precompiled.h"

#include <Engine/Fence.h>
#include <Engine/Gfx.h>

namespace gfx
{
	void InitFence(Fence& fence)
	{
		plat::InitFence(fence);
	}

	void DestroyFence(Fence& fence)
	{
		plat::DestroyFence(fence);
	}

	void TriggerFence(gfx::Fence& fence, const uint64_t value, const CommandQueueIndex queue)
	{
		plat::TriggerFence(fence, value, queue);
	}

	void CPUWaitOnFence(gfx::Fence& fence, const uint64_t value)
	{
		plat::CPUWaitOnFence(fence, value);
	}

	void GPUStallOnFence(gfx::Fence& fence, const uint64_t value)
	{
		plat::GPUStallOnFence(fence, value);
	}
}