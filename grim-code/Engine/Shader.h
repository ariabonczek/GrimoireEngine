#pragma once

#if PLATFORM_PC
#include <Engine/DXShader.h>
#endif

namespace gfx
{
	enum ShaderStages
	{
		kVertex = 1,
		kPixel = 2,
		kCompute = 4
	};
	struct Shader
	{
		int shaderStages;
#if PLATFORM_PC
		plat::DXShader platformShader;
#endif
	};

	void LoadShader(Shader& shader, const char* shaderPath);
	void DestroyShader(Shader& shader);
}