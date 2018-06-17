#include "Precompiled.h"

#include <Engine/CommandContext.h>
#include <Engine/DXCommandContext.h>

#include <Engine/DXGfx.h>
#include <Engine/DXSurface.h>
#include <Engine/EngineGlobals.h>

#include <Engine/d3dx12.h>
#include <d3dcompiler.h>

namespace plat
{
	void InitCommandContext(gfx::CommandContext& context)
	{
		DXCommandContext& platformContext = context.platformContext;

		HRESULT hr = plat::GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&platformContext.m_commandAllocator));
		GRIM_ASSERT(!hr, "Failed to create Command Allocator");
		platformContext.m_commandAllocator->SetName(L"Command Allocator");

		hr = plat::GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, platformContext.m_commandAllocator, NULL, IID_PPV_ARGS(&platformContext.m_commandList));
		GRIM_ASSERT(!hr, "Failed to create Command List");
		platformContext.m_commandList->SetName(L"Command List");

		platformContext.m_commandList->Close();
	}

	void DestroyCommandContext(gfx::CommandContext& context)
	{
		DXCommandContext& platformContext = context.platformContext;

		platformContext.m_commandList->Release();
		platformContext.m_commandAllocator->Release();

		platformContext.m_commandList = nullptr;
		platformContext.m_commandAllocator = nullptr;
	}
}

namespace gfx
{
	void CommandContext::Reset()
	{
		CPUWaitForFence(m_expectedFenceValue);

		ID3D12GraphicsCommandList* ctx = platformContext.m_commandList;
		ID3D12CommandAllocator* alloc = platformContext.m_commandAllocator;

		HRESULT hr = alloc->Reset();
		GRIM_ASSERT(!hr, "Failed to reset CommandAllocator");

		hr = ctx->Reset(platformContext.m_commandAllocator, NULL);
		GRIM_ASSERT(!hr, "Failed to reset CommandList");

		memset(&m_graphicsState, 0, sizeof(GraphicsState));
		memset(&m_resourceState, 0, sizeof(ResourceState));
	}

	void CommandContext::Close()
	{
		ID3D12GraphicsCommandList* ctx = platformContext.m_commandList;

		for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			if (!m_renderTargets[i])
				continue;

			plat::DXSurface& surface = m_renderTargets[i]->m_platformSurface;
			ctx->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(surface.m_surfaceResource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON));
			surface.m_currentResourceState = D3D12_RESOURCE_STATE_COMMON;
		}

		if (m_depthRenderTarget)
		{
			plat::DXSurface& surface = m_depthRenderTarget->m_platformSurface;
			ctx->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(surface.m_surfaceResource, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COMMON));
			surface.m_currentResourceState = D3D12_RESOURCE_STATE_COMMON;
		}

		memset(m_renderTargets, 0, sizeof(Surface*) * 8);
		m_depthRenderTarget = nullptr;

		HRESULT hr = ctx->Close();
		GRIM_ASSERT(!hr, "Failed to close CommandList");
	}

	void CommandContext::Submit()
	{
		ID3D12GraphicsCommandList* ctx = platformContext.m_commandList;

		ID3D12CommandList* ppCommandLists[] = { ctx };
		plat::GetCommandQueue(gfx::kGfxQueue)->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		m_expectedFenceValue = InsertFence();
	}

	void CommandContext::SetViewport(const ViewPort& viewport)
	{
		ID3D12GraphicsCommandList* ctx = platformContext.m_commandList;
		
		CD3DX12_VIEWPORT m_viewport;
		m_viewport = CD3DX12_VIEWPORT(viewport.topLeftX, viewport.topLeftY, viewport.width, viewport.height, viewport.minDepth, viewport.maxDepth);
		ctx->RSSetViewports(1, &m_viewport);
	}

	void CommandContext::SetScissorRect()
	{
		ID3D12GraphicsCommandList* ctx = platformContext.m_commandList;
		
		CD3DX12_RECT m_scissorRect;
		m_scissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(g_engineGlobals.frameBufferWidth), static_cast<LONG>(g_engineGlobals.frameBufferWidth));
		ctx->RSSetScissorRects(1, &m_scissorRect);
	}

	void CommandContext::SetRenderTarget(const int index, Surface* renderTarget)
	{
		ID3D12GraphicsCommandList* ctx = platformContext.m_commandList;
		
		GRIM_ASSERT(index < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT, "Render target index out of bounds");

		if (renderTarget == m_renderTargets[index])
			return;

		if (m_renderTargets[index])
		{
			plat::DXSurface& surface = m_renderTargets[index]->m_platformSurface;
			ctx->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(surface.m_surfaceResource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON));
			surface.m_currentResourceState = D3D12_RESOURCE_STATE_COMMON;
		}
		
		if (renderTarget)
		{
			plat::DXSurface& surface = renderTarget->m_platformSurface;
			ctx->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(surface.m_surfaceResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET));
			surface.m_currentResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;
		}

		m_renderTargets[index] = renderTarget;
	}

	void CommandContext::SetDepthTarget(Surface* depthTarget)
	{
		ID3D12GraphicsCommandList* ctx = platformContext.m_commandList;

		if (depthTarget == m_depthRenderTarget)
			return;

		if (m_depthRenderTarget)
		{
			plat::DXSurface& surface = m_depthRenderTarget->m_platformSurface;
			ctx->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(surface.m_surfaceResource, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COMMON));
			surface.m_currentResourceState = D3D12_RESOURCE_STATE_COMMON;
		}

		if (depthTarget)
		{
			plat::DXSurface& surface = depthTarget->m_platformSurface;
			ctx->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(surface.m_surfaceResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
			surface.m_currentResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		}
		m_depthRenderTarget = depthTarget;
	}

	void CommandContext::SetGraphicsState(const GraphicsState& graphicsState)
	{
		m_graphicsState = graphicsState;

		for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			SetRenderTarget(i, nullptr);
		}
		SetDepthTarget(nullptr);

		ID3D12GraphicsCommandList* ctx = platformContext.m_commandList;
		ctx->IASetPrimitiveTopology((D3D12_PRIMITIVE_TOPOLOGY) graphicsState.primMode);
	}

	void CommandContext::SetResourceState(const ResourceState& resourceState)
	{
		m_resourceState = resourceState;

		ID3D12GraphicsCommandList* ctx = platformContext.m_commandList;

		ctx->SetGraphicsRootSignature(m_resourceState.shader->platformShader.rootSignature);
		//ctx->SetPipelineState(m_resourceState.);
	}

	void CommandContext::HACK_SetPipelineState(void* pipelineState)
	{
		ID3D12PipelineState* state = (ID3D12PipelineState*)pipelineState;
		ID3D12GraphicsCommandList* ctx = platformContext.m_commandList;

		ctx->SetPipelineState(state);
	}

	void CommandContext::ClearRenderTarget(int index, const float color[4])
	{
		GRIM_ASSERT(m_renderTargets[index] && !m_renderTargets[index]->m_isDepthTarget, "Cannot clear a null render target or depth target");

		ID3D12GraphicsCommandList* ctx = platformContext.m_commandList;

		ctx->ClearRenderTargetView(m_renderTargets[index]->m_platformSurface.m_surfaceHandle, color, 0, nullptr);
	}

	void CommandContext::ClearDepthTarget(const float clearValue)
	{
		GRIM_ASSERT(m_depthRenderTarget && m_depthRenderTarget->m_isDepthTarget, "Cannot clear a null render target or color target");

		ID3D12GraphicsCommandList* ctx = platformContext.m_commandList;

		ctx->ClearDepthStencilView(m_depthRenderTarget->m_platformSurface.m_surfaceHandle, D3D12_CLEAR_FLAG_DEPTH, clearValue, 0, 0, nullptr);
	}

	void CommandContext::DrawInstanced(uint32_t numVertsPerInstance, uint32_t numInstances, uint32_t startVertIndex, uint32_t startInstanceIndex)
	{
		ID3D12GraphicsCommandList* ctx = platformContext.m_commandList;

		UpdatePipelineState();

		ctx->DrawInstanced(numVertsPerInstance, numInstances, startVertIndex, startInstanceIndex);
	}

	void CommandContext::DrawIndexed(uint32_t numIndices)
	{
		ID3D12GraphicsCommandList* ctx = platformContext.m_commandList;

		UpdatePipelineState();

		ctx->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
	}

	void CommandContext::DrawInstancedIndexed(uint32_t numIndicesPerInstance, uint32_t numInstances, uint32_t startVertIndex, uint32_t startInstanceIndex)
	{
		ID3D12GraphicsCommandList* ctx = platformContext.m_commandList;

		UpdatePipelineState();

		ctx->DrawIndexedInstanced(numIndicesPerInstance, numInstances, startVertIndex, 0, startInstanceIndex);
	}

	void CommandContext::UpdatePipelineState()
	{
		ID3D12GraphicsCommandList* ctx = platformContext.m_commandList;

		D3D12_PRIMITIVE_TOPOLOGY primMode = m_graphicsState.primMode == kTriangleList ? D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST : D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		ctx->IASetPrimitiveTopology(primMode);
		ctx->IASetVertexBuffers(0, 1, &m_resourceState.vertexBuffer->m_platformBuffer.m_vertexBufferView);
		ctx->IASetIndexBuffer(&m_resourceState.indexBuffer->m_platformBuffer.m_indexBufferView);
		
		ID3D12DescriptorHeap* ppHeaps[] = { m_resourceState.shaderResourceTable->m_platformBuffer.m_cbvHeap };
		ctx->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		ctx->SetGraphicsRootDescriptorTable(0, m_resourceState.shaderResourceTable->m_platformBuffer.m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
		
		// Render targets
		D3D12_CPU_DESCRIPTOR_HANDLE handles[8];
		int index = 0;
		for (int i = 0; i < 8; ++i)
		{
			plat::DXSurface& renderTarget = m_renderTargets[i]->m_platformSurface;
			if (m_renderTargets[i])
			{
				handles[index++] = renderTarget.m_surfaceHandle;
			}
		}
		ctx->OMSetRenderTargets(index, handles, FALSE, m_depthRenderTarget ? &m_depthRenderTarget->m_platformSurface.m_surfaceHandle : nullptr);		
	}
}