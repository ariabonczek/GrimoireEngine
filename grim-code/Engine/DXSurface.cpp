#include "Precompiled.h"

#include <Engine/Surface.h>

#include <Engine/Gfx.h>
#include <Engine/DXGfx.h>

struct SurfaceHeap
{
	ID3D12DescriptorHeap* rtvHeap;
	ID3D12DescriptorHeap* dsvHeap;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandleToStart;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandleToEnd;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandleToStart;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandleToEnd;
};

static SurfaceHeap s_surfaceHeap;
static uint32_t s_rtvDescriptorSize;
static uint32_t s_dsvDescriptorSize;


void plat::CreateSurface(gfx::Surface& surface)
{
	const gfx::SurfaceDefinition& surfaceDefinition = surface.m_definition;
	DXSurface& currSurface = surface.m_platformSurface;

	const DXGI_FORMAT format = (DXGI_FORMAT) surfaceDefinition.format;
	bool isDepth = (format == DXGI_FORMAT_D32_FLOAT || format == DXGI_FORMAT_D24_UNORM_S8_UINT);
	ID3D12Device* device = plat::GetDevice();

	{
		// Create the resource
		D3D12_RESOURCE_DESC desc = {};
		desc.Format = format;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width = surfaceDefinition.width;
		desc.Height = surfaceDefinition.height;
		desc.MipLevels = 0;
		desc.DepthOrArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Flags = isDepth ?
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL :
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_HEAP_PROPERTIES properties;
		properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		properties.CreationNodeMask = 1;
		properties.VisibleNodeMask = 1;

		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = format;
		if (isDepth)
		{
			clearValue.DepthStencil.Depth = 1.0f;
			clearValue.DepthStencil.Stencil = 0;
		}
		else
		{
			memset(clearValue.Color, 0, sizeof(FLOAT) * 4);
		}

		HRESULT hr = device->CreateCommittedResource(&properties, D3D12_HEAP_FLAG_NONE, &desc,
			D3D12_RESOURCE_STATE_COMMON, &clearValue, IID_PPV_ARGS(&currSurface.m_surfaceResource));

		GRIM_ASSERT(!hr, "Failed to create resource for surface %s", surface.m_definition.name);
		
		wchar_t name[32];
		mbstowcs(name, surface.m_definition.name, 32);
		currSurface.m_surfaceResource->SetName(name);
	}
	
	if (isDepth)
	{
		// Create the RenderTargetView
		D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format = format;
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		desc.Flags = D3D12_DSV_FLAG_NONE;
		desc.Texture2D.MipSlice = 0;
		
		D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle = s_surfaceHeap.dsvHandleToEnd;
		device->CreateDepthStencilView(currSurface.m_surfaceResource, &desc, dsvHandle);
		
		currSurface.m_surfaceHandle = dsvHandle;
		dsvHandle.ptr += s_dsvDescriptorSize;
	}
	else
	{
		// Create the RenderTargetView
		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format = format;
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;
		desc.Texture2D.PlaneSlice = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE& rtvHandle = s_surfaceHeap.rtvHandleToEnd;
		device->CreateRenderTargetView(currSurface.m_surfaceResource, &desc, rtvHandle);
		
		currSurface.m_surfaceHandle = rtvHandle;
		rtvHandle.ptr +=s_rtvDescriptorSize;
	}

	currSurface.m_currentResourceState = D3D12_RESOURCE_STATE_COMMON;
}

void plat::DestroySurface(gfx::Surface& surface)
{
	GRIM_ASSERT(surface.m_platformSurface.m_surfaceResource, "Attempt made to destroy surface %s which was never created", surface.m_definition.name);
	
	surface.m_platformSurface.m_surfaceResource->Release();
	surface.m_platformSurface.m_surfaceResource = nullptr;
}


void gfx::InitFramebufferSurfaces(gfx::Surface* surfaces)
{
	// RTV Heap
	{
		s_rtvDescriptorSize = plat::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = kNumDisplayBuffers;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		HRESULT hr = plat::GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&s_surfaceHeap.rtvHeap));
		GRIM_ASSERT(!hr, "Failed to create render target heap");
		s_surfaceHeap.rtvHeap->SetName(L"Surface RTV Heap");

		s_surfaceHeap.rtvHandleToStart = CD3DX12_CPU_DESCRIPTOR_HANDLE(s_surfaceHeap.rtvHeap->GetCPUDescriptorHandleForHeapStart());
		s_surfaceHeap.rtvHandleToEnd = s_surfaceHeap.rtvHandleToStart;
	}

	// DSV Heap
	{
		s_dsvDescriptorSize = plat::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = kNumDisplayBuffers;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		HRESULT hr = plat::GetDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&s_surfaceHeap.dsvHeap));
		GRIM_ASSERT(!hr, "Failed to create depth target heap");
		s_surfaceHeap.dsvHeap->SetName(L"Surface DSV Heap");

		s_surfaceHeap.dsvHandleToStart = CD3DX12_CPU_DESCRIPTOR_HANDLE(s_surfaceHeap.dsvHeap->GetCPUDescriptorHandleForHeapStart());
		s_surfaceHeap.dsvHandleToEnd = s_surfaceHeap.dsvHandleToStart;
	}

	// Create resource views for the display buffers from the swap chain

	for (int i = 0; i < kNumDisplayBuffers; ++i)
	{
		gfx::Surface& surface = surfaces[i];
		plat::DXSurface& platformSurface = surfaces[i].m_platformSurface;
		D3D12_CPU_DESCRIPTOR_HANDLE& rtvHandle = s_surfaceHeap.rtvHandleToEnd;

		HRESULT hr = plat::GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&platformSurface.m_surfaceResource));
		GRIM_ASSERT(!hr, "Failed to get display buffer resource");

		char cName[32] = "";
		snprintf(cName, 32, "%s %d", "Framebuffer", i);

		wchar_t name[32];
		mbstowcs(name, cName, 32);
		platformSurface.m_surfaceResource->SetName(name);

		plat::GetDevice()->CreateRenderTargetView(platformSurface.m_surfaceResource, nullptr, rtvHandle);

		platformSurface.m_surfaceHandle = rtvHandle;
		rtvHandle.ptr += s_rtvDescriptorSize;

		D3D12_RESOURCE_DESC desc = platformSurface.m_surfaceResource->GetDesc();

		gfx::SurfaceDefinition& def = surface.m_definition;
		def.format = (gfx::DataFormat) desc.Format;
		def.width = (uint32_t)desc.Width;
		def.height = (uint32_t)desc.Height;
	}
}

void gfx::ResizeFramebuffer(const uint32_t width, const uint32_t height)
{
	printf("Detected a change in window size, but resizing engine surfaces is still unimplemented on this platform\n");
}

void gfx::DestroyFramebufferSurfaces(gfx::Surface* surfaces)
{
	// Release descriptor heaps
	s_surfaceHeap.rtvHeap->Release();
	s_surfaceHeap.dsvHeap->Release();

	for (int i = 0; i < kNumDisplayBuffers; ++i)
	{
		plat::DestroySurface(surfaces[i]);
	}
}