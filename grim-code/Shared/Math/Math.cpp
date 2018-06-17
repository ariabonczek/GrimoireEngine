#include "MathCommon.h"

NS_MATH

float DegToRad(float d)
{
	return PI * d / 180.0f;
}

float RadToDeg(float r)
{
	return r * 180.0f / PI;
}

NS_MATH_END