#include "Precompiled.h"

#include <Engine/Shader.h>

namespace gfx
{
	void LoadShader(Shader& shader, const char* shaderPath)
	{
		plat::LoadShader(shader, shaderPath);
	}
	void DestroyShader(Shader& shader)
	{
		plat::DestroyShader(shader);
	}
}