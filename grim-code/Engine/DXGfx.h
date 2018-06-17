#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <Engine/Gfx.h>
#include <Engine/Surface.h>

namespace plat 
{
	ID3D12Device* GetDevice();
	ID3D12CommandQueue* GetCommandQueue(int index);
	IDXGISwapChain3* GetSwapChain();

	void InitGfx();
	void ProcessGfx();
	void ShutdownGfx();
	void Flip();

	int GetCurrentBackbufferIndex();
	void SetWindowTitle(const char* text);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC StateToPipelineDesc(const gfx::GraphicsState& graphicsState, const gfx::ResourceState& resourceState, const gfx::Surface* renderTargets[8], const gfx::Surface* depthRenderTarget);

}