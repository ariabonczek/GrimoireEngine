#pragma once

#include <d3d12.h>

namespace gfx
{
	class CommandContext;
}

namespace plat
{
	struct DXCommandContext
	{
		ID3D12GraphicsCommandList* m_commandList;
		ID3D12CommandAllocator* m_commandAllocator;
	};

	void InitCommandContext(gfx::CommandContext& context);
	void DestroyCommandContext(gfx::CommandContext& context);

}