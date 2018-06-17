#include "Precompiled.h"

#include <Engine/ForwardLighting.h>

extern gfx::DrawElement* s_drawElements;
gfx::ForwardLighting_Data s_cheat; // allocate out of per-frame scratch buffer


size_t gfx::ForwardLighting_GetData()
{
	ForwardLighting_Data* data = &s_cheat; // allocate out of per-frame scratch buffer

	data->drawElements = s_drawElements;// we'll figure this out later
	data->numElements = 1; // we'll figure this out later

	return (size_t)data;
}

void gfx::ForwardLighting_Execute(gfx::CommandContext* ctx, gfx::RenderPass passDef, size_t data)
{
	gfx::BeginPass(passDef, ctx);

	ForwardLighting_Data* passData = (ForwardLighting_Data*)data;
	
	for (int i = 0; i < passData->numElements;  ++i)
	{
		const DrawElement& element = passData->drawElements[i];

		ctx->SetResourceState(element.resourceState);
		ctx->DrawIndexed(element.indexCount);
	}
}