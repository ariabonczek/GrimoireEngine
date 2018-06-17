#include "Precompiled.h"

#include <Engine/Buffer.h>
#include <Engine/DXGfx.h>
#include <Engine/d3dx12.h>

namespace plat
{
	void CreateBuffer(gfx::BufferDefinition& definition, gfx::Buffer& buffer)
	{
		DXBuffer& platformBuffer = buffer.m_platformBuffer;

		if(definition.bufferType == gfx::kConstantBuffer)
			definition.sizeAlign.size = (definition.sizeAlign.size + 255) & ~255;	// CB size is required to be 256-byte aligned.

		D3D12_RESOURCE_DESC desc = {};
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = definition.sizeAlign.size;
		desc.Height = 1;
		desc.Alignment = definition.sizeAlign.align;
		desc.MipLevels = 1;
		desc.DepthOrArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_HEAP_PROPERTIES heapProperties;
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // TODO: NO UPLOAD HEAPS
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;		

		ID3D12Resource*& bufferResource = platformBuffer.m_bufferResource;
		HRESULT hr = plat::GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferResource));
		GRIM_ASSERT(!hr, "Failed to create buffer");
#if _DEBUG
		wchar_t name[32];
		mbstowcs(name, definition.name, 32);
		bufferResource->SetName(name);
#endif

		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
		cbvHeapDesc.NumDescriptors = 1;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		plat::GetDevice()->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&platformBuffer.m_cbvHeap));

		plat::UploadBufferData(buffer);

		D3D12_VERTEX_BUFFER_VIEW& vbv = platformBuffer.m_vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW& ibv = platformBuffer.m_indexBufferView;
		D3D12_CONSTANT_BUFFER_VIEW_DESC& cbv = platformBuffer.m_constantBufferView;

		switch (definition.bufferType)
		{
			case gfx::kVertexBuffer:
	
				vbv.BufferLocation = platformBuffer.m_bufferResource->GetGPUVirtualAddress();
				vbv.StrideInBytes = definition.stride;
				vbv.SizeInBytes = definition.sizeAlign.size;

				break;
			case gfx::kIndexBuffer:

				ibv.BufferLocation = platformBuffer.m_bufferResource->GetGPUVirtualAddress();
				ibv.SizeInBytes = definition.sizeAlign.size;
				ibv.Format = DXGI_FORMAT_R16_UINT;

				break;
			case gfx::kConstantBuffer:

				cbv.BufferLocation = bufferResource->GetGPUVirtualAddress();
				cbv.SizeInBytes = definition.sizeAlign.size;	// CB size is required to be 256-byte aligned.

				plat::GetDevice()->CreateConstantBufferView(&cbv, platformBuffer.m_cbvHeap->GetCPUDescriptorHandleForHeapStart());

				break;
		}
	}

	void DestroyBuffer(gfx::Buffer& buffer)
	{
		buffer.m_platformBuffer.m_bufferResource->Release();
		buffer.m_platformBuffer.m_cbvHeap->Release();

		buffer.m_platformBuffer.m_bufferResource = nullptr;
		buffer.m_platformBuffer.m_cbvHeap = nullptr;
	}

	void UploadBufferData(gfx::Buffer& buffer)
	{
		gfx::BufferDefinition& definition = buffer.m_definition;
		DXBuffer dxBuffer = buffer.m_platformBuffer;

		// Copy the buffer data
		if (definition.baseAddress)
		{
			UINT8* bufferDataBegin;
			CD3DX12_RANGE readRange(0, 0);
			dxBuffer.m_bufferResource->Map(0, &readRange, reinterpret_cast<void**>(&bufferDataBegin));
			memcpy(bufferDataBegin, definition.baseAddress, definition.sizeAlign.size);
			dxBuffer.m_bufferResource->Unmap(0, nullptr);
		}
	}
}