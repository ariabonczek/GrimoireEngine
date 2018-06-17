#include "Precompiled.h"

#include <Engine/Surface.h>

namespace gfx 
{
	void CreateSurface(SurfaceDefinition& surfaceDefinition, Surface& surface)
	{
		surface.m_definition = surfaceDefinition;
		surface.m_isDepthTarget = surfaceDefinition.format == kDepth32 || surfaceDefinition.format == kDepth24Stencil8;

		plat::CreateSurface(surface);
	}

	void DestroySurface(gfx::Surface& surface)
	{
		plat::DestroySurface(surface);
	}
}