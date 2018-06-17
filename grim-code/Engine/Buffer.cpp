#include "Precompiled.h"

#include <Engine/Buffer.h>

namespace gfx
{
	void CreateBuffer(BufferDefinition& definition, Buffer& buffer)
	{
		buffer.m_definition = definition;

		plat::CreateBuffer(definition, buffer);
	}

	BufferType GetBufferTypeFromString(const char* string)
	{
		if (strcmp(string, "rwbuffer") == 0)
			return kRWBuffer;
		return kUnknown;
	}

	void DestroyBuffer(Buffer& buffer)
	{
		plat::DestroyBuffer(buffer);
	}

	void UploadBufferData(gfx::Buffer& buffer)
	{
		plat::UploadBufferData(buffer);
	}
}