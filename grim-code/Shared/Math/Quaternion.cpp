#include "Quaternion.h"
#include "Vector.h"
#include "Matrix.h"

NS_MATH

Quaternion Quaternion::kIdentity(0.0f, 0.0f, 0.0f, 1.0f);

Quaternion::Quaternion():
w(1.0f), x(), y(), z()
{

}

Quaternion::Quaternion(float x, float y, float z, float w):
w(w), x(x), y(y), z(z)
{

}

Quaternion::Quaternion(Vector3 v, float w):
w(w), x(v.x), y(v.y), z(v.z)
{

}

Quaternion::Quaternion(Vector4 v):
x(v.x), y(v.y), z(v.z), w(v.w)
{

}

Quaternion Quaternion::CreateFromAxisAngle(Vector3 axis, float angle)
{
	float half = DegToRad(angle) * 0.5f;
	float sin = sinf(half);
	float cos = cosf(half);
	return Quaternion(axis.x * sin, axis.y * sin, axis.z * sin, cos);
}

Quaternion Quaternion::CreateFromRotationMatrix(Matrix m)
{
	Quaternion q;
	float sqrt;
	float half;
	float scale = m.row[0].x + m.row[1].y + m.row[2].z + m.row[3].w;
	if (scale > 0.0f)
	{
		sqrt = sqrtf(scale + 1.0f);
		q.w = sqrt * 0.5f;
		sqrt = 0.5f / sqrt;

		q.x = (m.row[1].z - m.row[2].y) * sqrt;
		q.y = (m.row[2].x - m.row[0].z) * sqrt;
		q.z = (m.row[0].y - m.row[1].x) * sqrt;

		return q.Normalized();
	}
	else if ((m.row[0].x >= m.row[1].y) && (m.row[0].x <= m.row[2].z))
	{
		sqrt = sqrtf(1.0f + m.row[0].x - m.row[1].y - m.row[2].z);
		half = 0.5f / sqrt;

		q.x = 0.5f * sqrt;
		q.y = (m.row[0].y + m.row[1].x) * half;
		q.z = (m.row[0].z + m.row[2].x) * half;
		q.w = (m.row[1].z - m.row[2].y) * half;

		return q.Normalized();
	}
	else if (m.row[1].y > m.row[2].z)
	{
		sqrt = sqrtf(1.0f + m.row[1].y - m.row[1].y - m.row[0].x);
		half = 0.5f / sqrt;

		q.x = (m.row[1].x + m.row[0].y) * half;
		q.y = 0.5f * sqrt;
		q.z = (m.row[2].y + m.row[1].z) * half;
		q.w = (m.row[2].x - m.row[0].z) * half;

		return q.Normalized();
	}
	else
	{
		sqrt = sqrtf(1.0f + m.row[2].z - m.row[1].y - m.row[0].x);
		half = 0.5f / sqrt;

		q.x = (m.row[2].x + m.row[0].z) * half;
		q.y = (m.row[2].y + m.row[1].z) * half;
		q.z = 0.5f * sqrt;
		q.w = (m.row[0].y - m.row[1].x) * half;

		return q.Normalized();
	}
}

Quaternion Quaternion::CreateFromEulerAngles(Vector3 v)
{
	float y = DegToRad(v.y) * 0.5f;
	float x = DegToRad(v.x) * 0.5f;
	float z = DegToRad(v.z) * 0.5f;
	
	float siny = sinf(y);
	float cosy = cosf(y);
	float sinx = sinf(x);
	float cosx = cosf(x);
	float sinz = sinf(z);
	float cosz = cosf(z);

	return Quaternion(
		(cosy * sinx * cosz) + (siny * cosx * sinz),
		(siny * cosx * cosz) - (cosy * sinx * sinz),
		(cosy * cosx * sinz) - (siny * sinx * cosz),
		(cosy * cosx * cosz) + (siny * sinx * sinz)).Normalized();
}

Quaternion Quaternion::Lerp(Quaternion q1, Quaternion q2, float t)
{
	float inv = 1.0f - t;
	Quaternion ret;
	float mult = q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
	if (mult > 0.0f)
	{
		ret = Quaternion(inv*q1.x + t*q2.x, inv*q1.y + t*q2.y, inv*q1.z + t*q2.z, inv*q1.w + t*q2.w);
	}
	else
	{
		ret = Quaternion(inv*q1.x - t*q2.x, inv*q1.y - t*q2.y, inv*q1.z - t*q2.z, inv*q1.w - t*q2.w);
	}

	return ret.Normalized();
}

Quaternion Quaternion::Slerp(Quaternion q1, Quaternion q2, float t)
{
	float inv;
	float sign;

	float mult = q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
	bool flag = false;
	if (mult < 0.0f)
	{
		flag = true;
	}
	if (mult > 0.99999f)
	{
		inv = 1.0f - t;
		sign = flag ? -t : t;
	}
	else
	{
		float acos = acosf(mult);
		float invsin = 1.0f / sinf(acos);
		inv = sinf((1.0f - t) * acos) * invsin;
		sign = flag ? -sinf(t * acos) * invsin : sinf(t * acos) * invsin;
	}

	return Quaternion(inv*q1.x + sign*q2.x, inv*q1.y + sign*q2.y, inv*q1.z + sign*q2.z, inv*q1.w + sign*q2.w);
}

Quaternion Quaternion::Normalized()const
{
	float denom = 1.0f / sqrtf((x*x) + (y*y) + (z*z) + (w*w));
	return Quaternion(x * denom, y * denom, z * denom, w * denom);
}

float Quaternion::Magnitude()const
{
	return Vector4(x, y, z, w).Length();
}

float Quaternion::Magnitude(Quaternion& q)
{
	return Vector4(q.x, q.y, q.z, q.w).Length();
}

Quaternion Quaternion::Inverse()const
{
	float m = 1.0f / Magnitude();
	return Quaternion(-x * m, -y * m, -z * m, w * m);
}

Quaternion Quaternion::Inverse(Quaternion& q)
{
	float m = 1.0f / Magnitude(q);
	return Quaternion(-q.x * m, -q.y * m, -q.z * m, q.w * m);
}

float Quaternion::Dot(Quaternion& q1, Quaternion& q2)
{
	return Vector4::Dot(Vector4(q1.x, q1.y, q1.z, q1.w), Vector4(q2.x, q2.y, q2.z, q2.w));
}

float Quaternion::operator[](const int index)const
{
	switch (index)
	{
	case 0:
		return w;
	case 1:
		return x;
	case 2:
		return y;
	case 3:
		return z;
	default:
		throw "IndexOutOfRangeException";
	}
}

Quaternion Quaternion::operator+(const Quaternion& q)const
{
	return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
}

Quaternion Quaternion::operator-()const
{
	return Quaternion(-x, -y, -z, -w);
}

Quaternion operator*(const Quaternion& l, const Quaternion& r)
{
	float x = (l.w*r.x + l.x*r.w + l.y*r.z - l.z*r.y);
	float y = (l.w*r.y - l.x*r.z + l.y*r.w + l.z*r.x);
	float z = (l.w*r.z + l.x*r.y - l.y*r.x + l.z*r.w);
	float w = (l.w*r.w - l.x*r.x - l.y*r.y - l.z*r.z);
	return Quaternion(x, y, z, w);
}

NS_MATH_END