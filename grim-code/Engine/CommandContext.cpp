#include "Precompiled.h"

#include <Engine/CommandContext.h>

namespace gfx 
{
	uint64_t CommandContext::InsertFence(CommandQueueIndex commandQueue)
	{
		const uint64_t value = m_fenceValues[commandQueue];
		gfx::TriggerFence(m_fence, value, commandQueue);

		m_fenceValues[commandQueue]++;

		return value;
	}

	void CommandContext::GPUWaitForFence(const uint64_t fenceValue, CommandQueueIndex commandQueue)
	{
		gfx::GPUStallOnFence(m_fence, fenceValue);
	}

	void CommandContext::GPUWaitForFence(Fence& fence, const uint64_t fenceValue, CommandQueueIndex commandQueue)
	{
		gfx::GPUStallOnFence(fence, fenceValue);
	}

	void CommandContext::CPUWaitForFence(const uint64_t fenceValue, CommandQueueIndex commandQueue)
	{
		gfx::CPUWaitOnFence(m_fence, fenceValue);
	}

	void CommandContext::CPUWaitForFence(Fence& fence, const uint64_t fenceValue, CommandQueueIndex commandQueue)
	{
		gfx::CPUWaitOnFence(fence, fenceValue);
	}

	void InitCommandContext(CommandContext& context)
	{
		plat::InitCommandContext(context);

		plat::InitFence(context.m_fence);
	}

	void DestroyCommandContext(CommandContext& context)
	{
		context.CPUWaitForFence(context.m_expectedFenceValue);

		plat::DestroyFence(context.m_fence);
		plat::DestroyCommandContext(context);
	}
}