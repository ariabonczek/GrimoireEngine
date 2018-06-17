#ifndef MATH_COMMON_HPP
#define MATH_COMMON_HPP

#define NS_MATH 
#define NS_MATH_END 

#include <algorithm>

NS_MATH
	struct Vector2;
	struct Vector3;
	struct Vector4;
	struct Matrix;
	struct Quaternion;

	float DegToRad(float d);
	float RadToDeg(float r);
NS_MATH_END

#define lerp(f1, f2, t) f1 + (f2 - f1) * t

template <typename T>
inline const T& min(const T& a, const T& b) {
	return !(b < a) ? a : b;
}

template <typename T>
inline const T& max(const T& a, const T& b) {
	return !(b < a) ? b : a;
}

typedef unsigned int uint;

#define PI 3.14159265f

#endif