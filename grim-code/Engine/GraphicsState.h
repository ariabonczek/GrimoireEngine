#pragma once

#if PLATFORM_PC
#include <engine/DXEnums.h>
#endif

namespace gfx
{
	struct Surface;	

	struct RasterizerState
	{
		static RasterizerState GetDefault();

		FillMode fillMode;
		CullMode cullMode;
		bool	 frontCounterClockwise;
	};
	
	struct BlendState
	{
		static BlendState GetDefault();

		bool blendEnable;
		BlendType srcColor;
		BlendType destColor;
		BlendFunc colorFunc;
		BlendType srcAlpha;
		BlendType destAlpha;
		BlendFunc alphaFunc;
	};

	struct MultisampleState
	{
		static MultisampleState GetDefault();

		uint32_t count;
		uint32_t quality;
	};	

	struct DepthStencilState
	{
		static DepthStencilState GetDefault();

		bool testEnabled;
		bool writeEnabled;
		DepthFunc depthFunc;
	};

	struct GraphicsState
	{
		static GraphicsState GetDefault();

		RasterizerState rasterizerState;
		DepthStencilState depthStencilState;
		MultisampleState multisampleState;
		BlendState blendState[8];
		PrimMode primMode;
	};
}