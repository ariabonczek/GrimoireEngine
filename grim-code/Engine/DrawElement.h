#pragma once

#include <Engine/Shader.h>
#include <Engine/Buffer.h>


namespace gfx
{	
	struct ResourceState
	{
		Shader* shader;
		Buffer* vertexBuffer;
		Buffer* indexBuffer;
		Buffer* shaderResourceTable; // This is incorrect
	};

	struct DrawElement
	{
		ResourceState resourceState;
		int indexCount;
	};

}