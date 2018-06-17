#pragma once

#include <Engine/CommandContext.h>
#include <Engine/DrawElement.h>
#include <Engine/RenderGraph.h>

namespace gfx
{
	struct ForwardLighting_Data
	{
		DrawElement* drawElements;
		int numElements;
	};

	size_t ForwardLighting_GetData();
	void ForwardLighting_Execute(CommandContext* ctx, const RenderPass passDef, size_t data);

}