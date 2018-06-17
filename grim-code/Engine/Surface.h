#pragma once

#include <Engine/Gfx.h>

#if PLATFORM_PC
#include <Engine/DXSurface.h>
#endif

namespace gfx
{
	struct SurfaceDefinition
	{
		enum 
		{
			kSurfaceNameMaxLength = 32
		};

		char name[kSurfaceNameMaxLength];
		uint32_t width;
		uint32_t height;
		DataFormat format;
	};

	struct Surface
	{
		SurfaceDefinition m_definition;
#if PLATFORM_PC
		plat::DXSurface m_platformSurface;
#endif

		bool m_isDepthTarget;
	};

	void CreateSurface(SurfaceDefinition& surfaceDefinition, Surface& surface);
	void DestroySurface(gfx::Surface& surface);

	// Found in platform surface cpp
	void InitFramebufferSurfaces(gfx::Surface* surfaces);
	void ResizeFramebuffer(const uint32_t width, const uint32_t height);
	void DestroyFramebufferSurfaces(gfx::Surface* surfaces);
}