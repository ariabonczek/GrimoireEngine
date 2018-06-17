#include "Precompiled.h"

#include <Engine/DXEnums.h>

// todo: no
gfx::DataFormat gfx::GetDataFormatFromString(const char* string)
{
	if (strcmp(string, "kDepth32") == 0)
		return kDepth32;
	return kUnknownFormat;
}

// todo: no
gfx::DepthFunc gfx::GetDepthFuncFromString(const char* string)
{
	if (strcmp(string, "kLessEqual") == 0)
		return kLessEqual;
	return kInvalidDepthFunc;
}