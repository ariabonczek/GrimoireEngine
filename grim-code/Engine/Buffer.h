#pragma once

#if PLATFORM_PC
#include <Engine/DXBuffer.h>
#endif

#include <Engine/Gfx.h>
#include <Shared/Core/SizeAlign.h>

namespace gfx
{
	enum BufferType
	{
		kUnknown,
		kConstantBuffer,
		kRWBuffer,
		kVertexBuffer,
		kIndexBuffer
	};

	BufferType GetBufferTypeFromString(const char* string);

	struct BufferDefinition
	{
		enum 
		{
			kBufferNameMaxLength = 32
		};
		char name[kBufferNameMaxLength];

		void* baseAddress;
		SizeAlign sizeAlign;
		BufferType bufferType;
		uint32_t stride;
	};

	struct Buffer
	{
		BufferDefinition m_definition;

#if PLATFORM_PC
		plat::DXBuffer m_platformBuffer;
#endif
	};

	void CreateBuffer(BufferDefinition& definition, Buffer& buffer);
	void DestroyBuffer(Buffer& buffer);
	void UploadBufferData(gfx::Buffer& buffer);
}