#pragma once

#include <dxgi1_4.h>
#include <d3d12.h>

namespace gfx {

	enum DataFormat
	{
		kUnknownFormat = DXGI_FORMAT_UNKNOWN,
		kRGBA8 = DXGI_FORMAT_R8G8B8A8_UNORM,
		kRGB10A2 = DXGI_FORMAT_R10G10B10A2_UNORM,
		kR11G11B10 = DXGI_FORMAT_R11G11B10_FLOAT,
		kDepth32 = DXGI_FORMAT_D32_FLOAT,
		kDepth24Stencil8 = DXGI_FORMAT_D24_UNORM_S8_UINT
	};

	DataFormat GetDataFormatFromString(const char* string);

	enum BlendType
	{
		kZero = D3D12_BLEND_ZERO,
		kOne = D3D12_BLEND_ONE,
		kSrcColor = D3D12_BLEND_SRC_COLOR,
		kInvSrcColor = D3D12_BLEND_INV_SRC_COLOR,
		kSrcAlpha = D3D12_BLEND_SRC_ALPHA,
		kInvSrcAlpha = D3D12_BLEND_INV_SRC_ALPHA,
		kDestAlpha = D3D12_BLEND_DEST_ALPHA,
		kInvDestAlpha = D3D12_BLEND_INV_DEST_ALPHA,
		kDestColor = D3D12_BLEND_DEST_COLOR,
		kInvDestColor = D3D12_BLEND_INV_DEST_COLOR,
		kSrcAlphaSat = D3D12_BLEND_SRC_ALPHA_SAT,
		kBlendFactor = D3D12_BLEND_BLEND_FACTOR,
		kInvBlendFactor = D3D12_BLEND_INV_BLEND_FACTOR
	};

	enum BlendFunc
	{
		kAdd = D3D12_BLEND_OP_ADD,
		kSubtract = D3D12_BLEND_OP_SUBTRACT,
		kInvSubtract = D3D12_BLEND_OP_REV_SUBTRACT,
		kOpMin = D3D12_BLEND_OP_MIN,
		kOpMax = D3D12_BLEND_OP_MAX
	};

	enum FillMode
	{
		kWireframe = D3D12_FILL_MODE_WIREFRAME,
		kSolid = D3D12_FILL_MODE_SOLID
	};

	enum CullMode
	{
		kNoCull = D3D12_CULL_MODE_NONE,
		kFront = D3D12_CULL_MODE_FRONT,
		kBack = D3D12_CULL_MODE_BACK
	};

	enum DepthFunc
	{
		kInvalidDepthFunc = 0,
		kNever = D3D12_COMPARISON_FUNC_NEVER,
		kLess = D3D12_COMPARISON_FUNC_LESS,
		kEqual = D3D12_COMPARISON_FUNC_EQUAL,
		kLessEqual = D3D12_COMPARISON_FUNC_LESS_EQUAL,
		kGreater = D3D12_COMPARISON_FUNC_GREATER,
		kNotEqual = D3D12_COMPARISON_FUNC_NOT_EQUAL,
		kGreaterEqual = D3D12_COMPARISON_FUNC_GREATER_EQUAL,
		kAlways = D3D12_COMPARISON_FUNC_ALWAYS
	};

	DepthFunc GetDepthFuncFromString(const char* string);


	enum PrimMode
	{
		kPoint = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
		kLine = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
		kTriangleList = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		kTriangleStrip = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH
	};
}