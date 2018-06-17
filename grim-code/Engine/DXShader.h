#pragma once

#include <d3d12.h>

namespace gfx
{
	struct Shader;
	enum ShaderStages;
}

namespace plat
{
	struct DXShader
	{
		ID3DBlob* vertexShader;
		ID3DBlob* pixelShader;
		ID3D12RootSignature* rootSignature;
		D3D12_INPUT_LAYOUT_DESC inputLayout;
	};

	void LoadShader(gfx::Shader& shader, const char* shaderPath);
	void DestroyShader(gfx::Shader& shader);
}