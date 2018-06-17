#include "Precompiled.h"

#include <Engine/Shader.h>
#include <Engine/DXGfx.h>
#include <d3dcompiler.h>
#include <string>

// Helper function for resolving the full path of assets.
std::wstring s2ws(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

static std::wstring GetAssetFullPath(LPCWSTR assetName)
{
	std::string datapath = getenv("DATA_PATH");
	std::wstring assetsPath = s2ws(datapath);
	return assetsPath + L"\\" + assetName;
}

namespace plat
{
	void LoadShader(gfx::Shader& shader, const char* shaderPath)
	{
		HRESULT hr;
		//Create an empty root signature.
		{
			D3D12_DESCRIPTOR_RANGE1 ranges[1];
			ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			ranges[0].NumDescriptors = 1;
			ranges[0].BaseShaderRegister = 0;
			ranges[0].RegisterSpace = 0;
			ranges[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
			ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			
			D3D12_ROOT_DESCRIPTOR_TABLE1 descriptorTable;
			descriptorTable.NumDescriptorRanges = 1;
			descriptorTable.pDescriptorRanges = ranges;

			D3D12_ROOT_PARAMETER1 rootParameters[1];
			rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			rootParameters[0].DescriptorTable = descriptorTable;
						  
			D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc;
			desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
			desc.Desc_1_1.NumParameters = 1;
			desc.Desc_1_1.NumStaticSamplers = 0;
			desc.Desc_1_1.pParameters = rootParameters;
			desc.Desc_1_1.pStaticSamplers = nullptr;
			desc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

			{
				// "better performance" by denying access to stages that aren't used
				desc.Desc_1_1.Flags |=
					D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;			}

			ID3DBlob* signature;
			ID3DBlob* error;
			hr = D3D12SerializeVersionedRootSignature(&desc, &signature, &error);
			GRIM_ASSERT(!hr, "Failed to serialize root signature, Error: %s", (char*)error->GetBufferPointer());
			if(error)error->Release();

			hr = plat::GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&shader.platformShader.rootSignature));
			GRIM_ASSERT(!hr, "Failed to create root signature");
		}

		{

			// Should probably convert to column major at some point due to comment found here: 
			// https://msdn.microsoft.com/en-us/library/windows/desktop/gg615083(v=vs.85).aspx
			// Under "D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR"
			UINT compileFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#if defined(_DEBUG)
			// Enable better shader debugging with the graphics debugging tools.
			compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			wchar_t path[64];
			mbstowcs(path, shaderPath, 64);

			ID3DBlob* errorMessages;
			if (shader.shaderStages & gfx::kVertex)
			{
				hr = D3DCompileFromFile(GetAssetFullPath(path).c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &shader.platformShader.vertexShader, &errorMessages);
				GRIM_ASSERT(!hr, "Failed to compile vertex shader %s, Debug Output:\n%s", path, (char*)errorMessages->GetBufferPointer());
			}
			if (shader.shaderStages & gfx::kPixel)
			{
				hr = D3DCompileFromFile(GetAssetFullPath(path).c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &shader.platformShader.pixelShader, &errorMessages);
				GRIM_ASSERT(!hr, "Failed to compile pixel shader %s, Debug Output:\n%s", path, (char*)errorMessages->GetBufferPointer());
			}

			// Define the vertex input layout.
			static D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,	      0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			shader.platformShader.inputLayout.NumElements = 3;
			shader.platformShader.inputLayout.pInputElementDescs = inputElementDescs;
		}
	}

	void DestroyShader(gfx::Shader& shader)
	{
		DXShader platformShader = shader.platformShader;
		platformShader.rootSignature->Release();
		platformShader.vertexShader->Release();
		platformShader.pixelShader->Release();

		platformShader.rootSignature = nullptr;
		platformShader.vertexShader = nullptr;
		platformShader.pixelShader = nullptr;
	}
}