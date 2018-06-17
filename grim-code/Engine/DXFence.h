#pragma once

#include <d3d12.h>

namespace gfx
{
	enum CommandQueueIndex;
	struct Fence;
	class CommandContext;
}

namespace plat
{
	struct DXFence
	{
		ID3D12Fence* m_fence;
		HANDLE m_fenceEvent;
	};

	void InitFence(gfx::Fence& fence);
	void DestroyFence(gfx::Fence& fence);
	void TriggerFence(gfx::Fence& fence, const uint64_t value, const gfx::CommandQueueIndex queue);
	void CPUWaitOnFence(gfx::Fence& fence, const uint64_t value);
	void GPUStallOnFence(gfx::Fence& fence, const uint64_t value);
}