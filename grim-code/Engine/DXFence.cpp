#include "Precompiled.h"

#include <Engine/Fence.h>
#include <Engine/Gfx.h>
#include <Engine/DXGfx.h>
#include <Engine/CommandContext.h>

namespace plat
{
	void InitFence(gfx::Fence& fence)
	{
		DXFence& platformFence = fence.platformFence;

		HRESULT hr = plat::GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&platformFence.m_fence));
		GRIM_ASSERT(!hr, "Failed to create fence");
		platformFence.m_fence->SetName((L"Fence"));

		platformFence.m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		GRIM_ASSERT(platformFence.m_fenceEvent != nullptr, "Failed to create fence event");
	}

	void DestroyFence(gfx::Fence& fence)
	{
		DXFence& platformFence = fence.platformFence;

		platformFence.m_fence->Release();
		platformFence.m_fence = nullptr;
	}

	void TriggerFence(gfx::Fence& fence, const uint64_t value, const gfx::CommandQueueIndex queue)
	{
		DXFence& platformFence = fence.platformFence;

		HRESULT hr = plat::GetCommandQueue(queue)->Signal(platformFence.m_fence, value);
		GRIM_ASSERT(!hr, "Failed to signal fence on command queue %d", queue);
	}

	void CPUWaitOnFence(gfx::Fence& fence, const uint64_t value)
	{
		DXFence& platformFence = fence.platformFence;

		const uint64_t completedValue = platformFence.m_fence->GetCompletedValue();
		if (completedValue < value)
		{
			platformFence.m_fence->SetEventOnCompletion(value, platformFence.m_fenceEvent);
			WaitForSingleObjectEx(platformFence.m_fenceEvent, INFINITE, FALSE);
		}
	}

	void GPUStallOnFence(gfx::Fence& fence, const uint64_t value)
	{
		DXFence& platformFence = fence.platformFence;
		
		plat::GetCommandQueue(gfx::kGfxQueue)->Wait(platformFence.m_fence, value);
	}
}