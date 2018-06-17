#pragma once

#include <Engine/Shader.h>
#include <Engine/Buffer.h>


namespace gfx
{
	struct MeshElement
	{
		Shader* shader;
		Buffer* vertexBuffer;
		Buffer* indexBuffer;
		int indexCount;
	};
}