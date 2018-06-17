#pragma once

#if PLATFORM_PC
#include <Engine/DXCommandContext.h>
#endif

#include <Engine/Fence.h>
#include <Engine/Gfx.h>
#include <Engine/GraphicsState.h>
#include <Engine/DrawElement.h>
#include <Engine/Shader.h>
#include <Engine/View.h>

namespace gfx 
{
	struct Surface;

	class CommandContext
	{
	public:
		void Reset();
		void Close();
		void Submit();

		void SetViewport(const ViewPort& viewport);
		void SetScissorRect();

		void SetRenderTarget(const int index, Surface* renderTarget);
		void SetDepthTarget(Surface* depthTarget);
		void SetGraphicsState(const GraphicsState& graphicsState);
		void SetResourceState(const ResourceState& resourceState);
		void HACK_SetPipelineState(void* pipelineState);

		void ClearRenderTarget(int index, const float color[4]);
		void ClearDepthTarget(const float clearValue);

		void DrawIndexed(uint32_t numIndices);
		void DrawInstanced(uint32_t numVertsPerInstance, uint32_t numInstances, uint32_t startVertIndex, uint32_t startInstanceIndex);
		void DrawInstancedIndexed(uint32_t numIndicesPerInstance, uint32_t numInstances, uint32_t startVertIndex, uint32_t startInstanceIndex);

		uint64_t InsertFence(CommandQueueIndex commandQueue = kGfxQueue);
		void GPUWaitForFence(const uint64_t fenceValue, CommandQueueIndex commandQueue = kGfxQueue);
		void GPUWaitForFence(Fence& fence, const uint64_t fenceValue, CommandQueueIndex commandQueue = kGfxQueue);
		void CPUWaitForFence(const uint64_t fenceValue, CommandQueueIndex commandQueue = kGfxQueue);
		void CPUWaitForFence(Fence& fence, const uint64_t fenceValue, CommandQueueIndex commandQueue = kGfxQueue);

		Surface* m_renderTargets[8];
		Surface* m_depthRenderTarget;
		GraphicsState m_graphicsState;
		ResourceState m_resourceState;
		Fence m_fence;
		uint64_t m_fenceValues[kNumCommandQueues];
		uint64_t m_expectedFenceValue;

#if PLATFORM_PC
		plat::DXCommandContext platformContext;
#endif
	private:
		void UpdatePipelineState();
	};

	void InitCommandContext(CommandContext& context);
	void DestroyCommandContext(CommandContext& context);
}