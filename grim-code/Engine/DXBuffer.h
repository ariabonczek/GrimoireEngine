#pragma once

#include <d3d12.h>

namespace gfx
{
	struct BufferDefinition;
	struct Buffer;
}

namespace plat
{
	struct DXBuffer
	{
		ID3D12Resource* m_bufferResource;
		ID3D12DescriptorHeap* m_cbvHeap; // may be the wrong place

		union 
		{
			D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
			D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
			D3D12_CONSTANT_BUFFER_VIEW_DESC m_constantBufferView;
			D3D12_CPU_DESCRIPTOR_HANDLE m_surfaceHandle;
		};
	};

	void CreateBuffer(gfx::BufferDefinition& definition, gfx::Buffer& buffer);
	void DestroyBuffer(gfx::Buffer& buffer);
	void UploadBufferData(gfx::Buffer& buffer);
}