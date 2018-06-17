#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include "MathCommon.h"

NS_MATH

struct Quaternion
{
	float x, y, z, w;

	Quaternion();
	Quaternion(float x, float y, float z, float w);
	Quaternion(Vector3 v, float w);
	Quaternion(Vector4 v);

	//
	// Static Instances
	//
	static Quaternion kIdentity;

	//
	// Member Functions
	//
	Quaternion Normalized()const;
	float Magnitude()const;
	Quaternion Inverse()const;

	//
	// Static Functions
	//
	static Quaternion CreateFromAxisAngle(Vector3 axis, float angle);
	static Quaternion CreateFromRotationMatrix(Matrix m);
	static Quaternion CreateFromEulerAngles(Vector3 v);
	static Quaternion Lerp(Quaternion q1, Quaternion q2, float amount);
	static Quaternion Slerp(Quaternion q1, Quaternion q2, float amount);

	static float Magnitude(Quaternion& q);
	static Quaternion Inverse(Quaternion& q);
	static Quaternion Multiply(Quaternion& q1, Quaternion& q2);
	static float Dot(Quaternion& q1, Quaternion& q2);

	Quaternion operator+(const Quaternion& q)const;
	float operator[](const int index)const;
	Quaternion operator-()const;
};

Quaternion operator*(const Quaternion& l, const Quaternion& r);


NS_MATH_END

#endif