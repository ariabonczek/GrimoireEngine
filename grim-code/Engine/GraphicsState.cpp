#include "Precompiled.h"

#include <Engine/GraphicsState.h>

gfx::RasterizerState gfx::RasterizerState::GetDefault()
{
	static RasterizerState kDefault = { kSolid, kBack, false };
	return kDefault;
}

gfx::BlendState gfx::BlendState::GetDefault()
{
	static BlendState kDefault = { false, kOne, kZero, kAdd, kOne, kZero, kAdd };
	return kDefault;
}

gfx::MultisampleState gfx::MultisampleState::GetDefault()
{
	static MultisampleState kDefault = { 1, 0 };
	return kDefault;
}

gfx::DepthStencilState gfx::DepthStencilState::GetDefault()
{
	static DepthStencilState kDefault = { false, false, kEqual };
	return kDefault;
}

gfx::GraphicsState gfx::GraphicsState::GetDefault()
{
	static GraphicsState kDefault = {
		RasterizerState::GetDefault(),
		DepthStencilState::GetDefault(),
		MultisampleState::GetDefault(),
		{
			BlendState::GetDefault(),
			BlendState::GetDefault(),
			BlendState::GetDefault(),
			BlendState::GetDefault(),
			BlendState::GetDefault(),
			BlendState::GetDefault(),
			BlendState::GetDefault(),
			BlendState::GetDefault()
		},
		kTriangleList
	};
	return kDefault;
}
