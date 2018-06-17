#include "Precompiled.h"

#include <Engine/EngineGlobals.h>

#include <Engine/Gfx.h>

EngineGlobals g_engineGlobals;

void EngineGlobals::SetWindowTitle(const char* title)
{
	GRIM_ASSERT(title != nullptr, "Cannot set title to null string");
	strncpy(g_engineGlobals.windowTitle, title, EngineGlobals::kMaxWindowTitleLength);

	gfx::SetWindowTitle(g_engineGlobals.windowTitle);
}