#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <Engine/d3dx12.h>
#include <Engine/DXEnums.h>

namespace gfx
{
	struct SurfaceDefinition;
	struct Surface;
}

namespace plat
{
	struct DXSurface
	{
		ID3D12Resource*				m_surfaceResource;
		D3D12_CPU_DESCRIPTOR_HANDLE m_surfaceHandle;
		D3D12_RESOURCE_STATES		m_currentResourceState;
	};

	void CreateSurface(gfx::Surface& surface);
	void DestroySurface(gfx::Surface& surface);
}