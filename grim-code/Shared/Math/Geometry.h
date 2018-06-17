#pragma once

#include "MathCommon.h"

NS_MATH

// xyz is plane normal, w is offset
struct Plane
{
	float x, y, z, w;
};

struct Frustum
{
	// near, far, left, right, top bottom
	enum
	{
		kNear,
		kFar,
		kLeft,
		kRight,
		kTop,
		kBottom,

		kNumFrustumPlanes
	};
	Plane planes[kNumFrustumPlanes];
};

NS_MATH_END