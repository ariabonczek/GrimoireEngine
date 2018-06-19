#include "Matrix.h"
#include "Vector.h"
#include "Quaternion.h"

NS_MATH

Matrix Matrix::kIdentity(
	1.0f, 0.0f, 0.0f, 0.0f, 
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
	);

Matrix::Matrix() :
	row{ Vector4(), Vector4(), Vector4(), Vector4() }
{}

Matrix::Matrix(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44) :
	row{ { m11, m12, m13, m14 }, { m21, m22, m23, m24 }, { m31, m32, m33, m34 }, { m41, m42, m43, m44 } }
{}

Matrix::Matrix(Vector4 rowX, Vector4 rowY, Vector4 rowZ, Vector4 rowW) :
	row{ rowX, rowY, rowZ, rowW }
{}


float Matrix::Determinant()const
{
	return row[0].x * DeterminantHelper(row[1].y, row[1].z, row[1].w, row[2].y, row[2].z, row[2].w, row[3].y, row[3].z, row[3].w) -
		row[0].y * DeterminantHelper(row[1].x, row[1].z, row[1].w, row[2].x, row[2].z, row[2].w, row[3].x, row[3].z, row[3].w) +
		row[0].z * DeterminantHelper(row[1].x, row[1].y, row[1].w, row[2].x, row[2].y, row[2].w, row[3].x, row[3].y, row[3].w) -
		row[0].w * DeterminantHelper(row[1].x, row[1].y, row[1].z, row[2].x, row[2].y, row[2].z, row[3].x, row[3].y, row[3].z);
}
 
Matrix  Matrix::Transpose()const
{
	return Matrix(row[0].x, row[1].x, row[2].x, row[3].x, row[0].y, row[1].y, row[2].y, row[3].y, row[0].z, row[1].z, row[2].z, row[3].z, row[0].w, row[1].w, row[2].w, row[3].w);
}

Vector4 Matrix::GetRowX()const
{
	return Vector4(row[0].x, row[0].y, row[0].z, row[0].w);
}

Vector4 Matrix::GetRowY()const
{
	return Vector4(row[1].x, row[1].y, row[1].z, row[1].w);
}

Vector4 Matrix::GetRowZ()const
{
	return Vector4(row[2].x, row[2].y, row[2].z, row[2].w);
}

Vector4 Matrix::GetRowW()const
{
	return Vector4(row[3].x, row[3].y, row[3].z, row[3].w);
}

void Matrix::SetRowX(Vector4 val)
{
	row[0].x = val.x; row[0].y = val.y; row[0].z = val.z; row[0].w = val.w;
}

void Matrix::SetRowY(Vector4 val)
{
	row[1].x = val.x; row[1].y = val.y; row[1].z = val.z; row[1].w = val.w;
}

void Matrix::SetRowZ(Vector4 val)
{
	row[2].x = val.x; row[2].y = val.y; row[2].z = val.z; row[2].w = val.w;
}

void Matrix::SetRowW(Vector4 val)
{
	row[3].x = val.x; row[3].y = val.y; row[3].z = val.z; row[3].w = val.w;
}

Matrix operator*(const Matrix& l, const Matrix& r)
{
	return Matrix(
		((l.row[0].x * r.row[0].x) + (l.row[0].y * r.row[1].x) + (l.row[0].z * r.row[2].x) + (l.row[0].w * r.row[3].x)),
		((l.row[0].x * r.row[0].y) + (l.row[0].y * r.row[1].y) + (l.row[0].z * r.row[2].y) + (l.row[0].w * r.row[3].y)),
		((l.row[0].x * r.row[0].z) + (l.row[0].y * r.row[1].z) + (l.row[0].z * r.row[2].z) + (l.row[0].w * r.row[3].z)),
		((l.row[0].x * r.row[0].w) + (l.row[0].y * r.row[1].w) + (l.row[0].z * r.row[2].w) + (l.row[0].w * r.row[3].w)),

		((l.row[1].x * r.row[0].x) + (l.row[1].y * r.row[1].x) + (l.row[1].z * r.row[2].x) + (l.row[1].w * r.row[3].x)),
		((l.row[1].x * r.row[0].y) + (l.row[1].y * r.row[1].y) + (l.row[1].z * r.row[2].y) + (l.row[1].w * r.row[3].y)),
		((l.row[1].x * r.row[0].z) + (l.row[1].y * r.row[1].z) + (l.row[1].z * r.row[2].z) + (l.row[1].w * r.row[3].z)),
		((l.row[1].x * r.row[0].w) + (l.row[1].y * r.row[1].w) + (l.row[1].z * r.row[2].w) + (l.row[1].w * r.row[3].w)),

		((l.row[2].x * r.row[0].x) + (l.row[2].y * r.row[1].x) + (l.row[2].z * r.row[2].x) + (l.row[2].w * r.row[3].x)),
		((l.row[2].x * r.row[0].y) + (l.row[2].y * r.row[1].y) + (l.row[2].z * r.row[2].y) + (l.row[2].w * r.row[3].y)),
		((l.row[2].x * r.row[0].z) + (l.row[2].y * r.row[1].z) + (l.row[2].z * r.row[2].z) + (l.row[2].w * r.row[3].z)),
		((l.row[2].x * r.row[0].w) + (l.row[2].y * r.row[1].w) + (l.row[2].z * r.row[2].w) + (l.row[2].w * r.row[3].w)),

		((l.row[3].x * r.row[0].x) + (l.row[3].y * r.row[1].x) + (l.row[3].z * r.row[2].x) + (l.row[3].w * r.row[3].x)),
		((l.row[3].x * r.row[0].y) + (l.row[3].y * r.row[1].y) + (l.row[3].z * r.row[2].y) + (l.row[3].w * r.row[3].y)),
		((l.row[3].x * r.row[0].z) + (l.row[3].y * r.row[1].z) + (l.row[3].z * r.row[2].z) + (l.row[3].w * r.row[3].z)),
		((l.row[3].x * r.row[0].w) + (l.row[3].y * r.row[1].w) + (l.row[3].z * r.row[2].w) + (l.row[3].w * r.row[3].w))
		);
}

bool Matrix::operator==(const Matrix& m)const
{
	return
		(row[0].x == m.row[0].x) && (row[0].y == m.row[0].y) && (row[0].z == m.row[0].z) && (row[0].w == m.row[0].w) &&
		(row[1].x == m.row[1].x) && (row[1].y == m.row[1].y) && (row[1].z == m.row[1].z) && (row[1].w == m.row[1].w) &&
		(row[2].x == m.row[2].x) && (row[2].y == m.row[2].y) && (row[2].z == m.row[2].z) && (row[2].w == m.row[2].w) &&
		(row[3].x == m.row[3].x) && (row[3].y == m.row[3].y) && (row[3].z == m.row[3].z) && (row[3].w == m.row[3].w);
}

Matrix operator*(const Matrix& m, float s)
{
	return Matrix(
		m.row[0].x * s, m.row[0].y * s, m.row[0].z *s, m.row[0].w * s,
		m.row[1].x * s, m.row[1].y * s, m.row[1].z *s, m.row[1].w * s,
		m.row[2].x * s, m.row[2].y * s, m.row[2].z *s, m.row[2].w *s,
		m.row[3].x * s, m.row[3].y * s, m.row[3].z *s, m.row[3].w * s
		);
}

Matrix operator*(float s, Matrix& m)
{
	return Matrix(
		m.row[0].x * s, m.row[0].y * s, m.row[0].z * s, m.row[0].w * s,
		m.row[1].x * s, m.row[1].y * s, m.row[1].z * s, m.row[1].w * s,
		m.row[2].x * s, m.row[2].y * s, m.row[2].z * s, m.row[2].w * s,
		m.row[3].x * s, m.row[3].y * s, m.row[3].z * s, m.row[3].w * s
		);
}

Matrix Matrix::CreateFromQuaternion(Quaternion q)
{
	float x2 = q.x*q.x;
	float y2 = q.y*q.y;
	float z2 = q.z*q.z;
	float xy = q.x*q.y;
	float zw = q.z*q.w;
	float xz = q.z*q.x;
	float yw = q.y*q.w;
	float yz = q.y*q.z;
	float xw = q.x*q.w;

	return Matrix(
		1.0f - (2.0f * (y2 + z2)), 2.0f * (xy + zw),          2.0f * (xz - yw), 0.0f,
		2.0f * (xy - zw),		   1.0f - (2.0f * (z2 + x2)), 2.0f * (yz + xw), 0.0f,
		2.0f * (xz + yw),		   2.0f * (yz - xw),          1.0f - (2.0f * (y2 + x2)), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix Matrix::CreateLookAt(Vector3 position, Vector3 target, Vector3 up)
{
	Vector3 lookForward = Vector3::Normalize(target - position);
	Vector3 lookRight = Vector3::Normalize(Vector3::Cross(up, lookForward));
	Vector3 lookUp = Vector3::Cross(lookForward, lookRight);

	return Matrix(
		Vector4(lookRight, 0.0f),
		Vector4(lookUp, 0.0f),
		Vector4(lookForward, 0.0f),
		Vector4(position, 1.0f));
}

Matrix Matrix::CreateOrthographic(float width, float height, float zNear, float zFar)
{
	if (zNear >= zFar)
		throw "[Matrix] Invalid zNear and zFar values.";

	return Matrix(2.0f / width, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / height, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f / (zNear - zFar), 0.0f,
		0.0f, 0.0f, zNear / (zNear - zFar), 1.0f);
}

Matrix Matrix::CreatePerspective(float width, float height, float zNear, float zFar)
{
	if (zNear >= zFar)
		throw "[Matrix] Invalid zNear and zFar values.";

	return Matrix(
		(2.0f * zNear) / width, 0.0f, 0.0f, 0.0f,
		0.0f, (2.0f * zNear) / height, 0.0f, 0.0f, 
		0.0f, 0.0f, zFar / (zFar - zNear), 1.0f, 
		0.0f, 0.0f, -(zNear * zFar) / (zFar - zNear), 0.0f);
}

Matrix Matrix::CreatePerspectiveFov(float fov, float aspect, float zNear, float zFar)
{
	if ((fov <= 0.0f) || (fov >= PI))
		throw "[Matrix] fov <= 0 or >= PI";
	if (zNear >= zFar)
		throw "[Matrix] Invalid zNear and zFar values.";

	float n = 1.0f / ((float)tanf(fov * 0.5f));
	float n2 = n / aspect;
	return Matrix(
		n2, 0.0f, 0.0f, 0.0f,
		0.0f, n, 0.0f, 0.0f,
		0.0f, 0.0f, zFar / (zFar - zNear), 1.0f,
		0.0f, 0.0f, -(zNear * zFar) / (zFar - zNear), 0.0f);
}

Matrix Matrix::CreateRotationByAxisAngle(Vector3 axis, float angle)
{
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;
	
	float sin = sinf(DegToRad(-angle));
	float cos = cosf(DegToRad(-angle));

	float x2 = x*x;
	float y2 = y*y;
	float z2 = z*z;

	float xy = x*y;
	float xz = x*z;
	float yz = y*z;

	return Matrix(x2 + (cos * (1.0f - x2)), (xy - (cos * xy)) + (sin * z), (xz - (cos * xz)) - (sin * y), 0.0f,
		(xy - (cos * xy)) - (sin * z), y2 + (cos * (1.0f - y2)), (yz - (cos * yz)) + sin * x, 0.0f,
		(xz - (cos * xz)) + sin * y, (yz - (cos * yz)) - sin * x, z2 + (cos * (1.0f - z2)), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);	
}

Matrix Matrix::CreateRotationX(float angle)
{
	float sin = sinf(DegToRad(-angle));
	float cos = cosf(DegToRad(-angle));

	return Matrix(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos, -sin, 0.0f,
		0.0f, sin, cos, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix Matrix::CreateRotationY(float angle)
{
	float sin = sinf(DegToRad(-angle));
	float cos = cosf(DegToRad(-angle));

	return Matrix(cos, 0.0f, sin, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sin, 0.0f, cos, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

}

Matrix Matrix::CreateRotationZ(float angle)
{
	float sin = sinf(DegToRad(-angle));
	float cos = cosf(DegToRad(-angle));

	return Matrix(cos, -sin, 0.0f, 0.0f,
		sin, cos, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

// TODO: Optimize this (super easy)
Matrix Matrix::CreateRotation(Vector3 rotation)
{
	Matrix x = CreateRotationX(rotation.x);
	Matrix y = CreateRotationY(rotation.y);
	Matrix z = CreateRotationZ(rotation.z);

	return z * x * y;
}

Matrix Matrix::CreateScale(float scale)
{
	return Matrix(scale, 0.0f, 0.0f, 0.0f,
		0.0f, scale, 0.0f, 0.0f,
		0.0f, 0.0f, scale, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix Matrix::CreateScale(float x, float y, float z)
{
	return Matrix(
		x, 0.0f, 0.0f, 0.0f,
		0.0f, y, 0.0f, 0.0f,
		0.0f, 0.0f, z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix Matrix::CreateScale(Vector3 scale)
{
	return Matrix(
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix Matrix::CreateTranslation(float x, float y, float z)
{
	return Matrix(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		x, y, z, 1.0f);
}

Matrix Matrix::CreateTranslation(Vector3 translation)
{
	return Matrix(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		translation.x, translation.y, translation.z, 1.0f);
}

Matrix Matrix::CreateWorld(Vector3 position, Vector3 forward, Vector3 up)
{
	Vector3 x, y, z;
	z = Vector3::Normalize(forward);
	x = Vector3::Normalize(Vector3::Cross(z, up));
	y = Vector3::Normalize(Vector3::Cross(x, z));
	
	return Matrix(
		x.x, x.y, x.z, 0.0f,
		y.x, y.y, y.z, 0.0f, 
		z.x, z.y, z.z, 0.0f,
		position.x, position.y, position.z, 1.0f);

}

Matrix Matrix::Inverse(Matrix m)
{
	// Source: https://github.com/mono/MonoGame/blob/develop/MonoGame.Framework/Matrix.cs
	float num1 =m.row[0].x;
	float num2 =m.row[0].y;
	float num3 =m.row[0].z;
	float num4 =m.row[0].w;
	float num5 =m.row[1].x;
	float num6 =m.row[1].y;
	float num7 =m.row[1].z;
	float num8 =m.row[1].w;
	float num9 =m.row[2].x;
	float num10 = m.row[2].y;
	float num11 = m.row[2].z;
	float num12 = m.row[2].w;
	float num13 = m.row[3].x;
	float num14 = m.row[3].y;
	float num15 = m.row[3].z;
	float num16 = m.row[3].w;
	float num17 = (float)((double)num11 * (double)num16 - (double)num12 * (double)num15);
	float num18 = (float)((double)num10 * (double)num16 - (double)num12 * (double)num14);
	float num19 = (float)((double)num10 * (double)num15 - (double)num11 * (double)num14);
	float num20 = (float)((double)num9 * (double)num16 - (double)num12 * (double)num13);
	float num21 = (float)((double)num9 * (double)num15 - (double)num11 * (double)num13);
	float num22 = (float)((double)num9 * (double)num14 - (double)num10 * (double)num13);
	float num23 = (float)((double)num6 * (double)num17 - (double)num7 * (double)num18 + (double)num8 * (double)num19);
	float num24 = (float)-((double)num5 * (double)num17 - (double)num7 * (double)num20 + (double)num8 * (double)num21);
	float num25 = (float)((double)num5 * (double)num18 - (double)num6 * (double)num20 + (double)num8 * (double)num22);
	float num26 = (float)-((double)num5 * (double)num19 - (double)num6 * (double)num21 + (double)num7 * (double)num22);
	float num27 = (float)(1.0 / ((double)num1 * (double)num23 + (double)num2 * (double)num24 + (double)num3 * (double)num25 + (double)num4 * (double)num26));
	float num28 = (float)((double)num7 * (double)num16 - (double)num8 * (double)num15);
	float num29 = (float)((double)num6 * (double)num16 - (double)num8 * (double)num14);
	float num30 = (float)((double)num6 * (double)num15 - (double)num7 * (double)num14);
	float num31 = (float)((double)num5 * (double)num16 - (double)num8 * (double)num13);
	float num32 = (float)((double)num5 * (double)num15 - (double)num7 * (double)num13);
	float num33 = (float)((double)num5 * (double)num14 - (double)num6 * (double)num13);
	float num34 = (float)((double)num7 * (double)num12 - (double)num8 * (double)num11);
	float num35 = (float)((double)num6 * (double)num12 - (double)num8 * (double)num10);
	float num36 = (float)((double)num6 * (double)num11 - (double)num7 * (double)num10);
	float num37 = (float)((double)num5 * (double)num12 - (double)num8 * (double)num9);
	float num38 = (float)((double)num5 * (double)num11 - (double)num7 * (double)num9);
	float num39 = (float)((double)num5 * (double)num10 - (double)num6 * (double)num9);

	return Matrix(num23 * num27,
		(float)-((double)num2 * (double)num17 - (double)num3 * (double)num18 + (double)num4 * (double)num19) * num27,
		(float)((double)num2 * (double)num28 - (double)num3 * (double)num29 + (double)num4 * (double)num30) * num27,
		(float)-((double)num2 * (double)num34 - (double)num3 * (double)num35 + (double)num4 * (double)num36) * num27,
		num24 * num27,
		(float)((double)num1 * (double)num17 - (double)num3 * (double)num20 + (double)num4 * (double)num21) * num27,
		(float)-((double)num1 * (double)num28 - (double)num3 * (double)num31 + (double)num4 * (double)num32) * num27,
		(float)((double)num1 * (double)num34 - (double)num3 * (double)num37 + (double)num4 * (double)num38) * num27,
		num25 * num27,
		(float)-((double)num1 * (double)num18 - (double)num2 * (double)num20 + (double)num4 * (double)num22) * num27,
		(float)((double)num1 * (double)num29 - (double)num2 * (double)num31 + (double)num4 * (double)num33) * num27,
		(float)-((double)num1 * (double)num35 - (double)num2 * (double)num37 + (double)num4 * (double)num39) * num27,
		num26 * num27,
		(float)((double)num1 * (double)num19 - (double)num2 * (double)num21 + (double)num3 * (double)num22) * num27,
		(float)-((double)num1 * (double)num30 - (double)num2 * (double)num32 + (double)num3 * (double)num33) * num27,
		(float)((double)num1 * (double)num36 - (double)num2 * (double)num38 + (double)num3 * (double)num39) * num27);
	
}

Matrix Matrix::Transpose(Matrix m)
{
	return Matrix(m.row[0].x, m.row[1].x, m.row[2].x, m.row[3].x, m.row[0].y, m.row[1].y, m.row[2].y, m.row[3].y, m.row[0].z, m.row[1].z, m.row[2].z, m.row[3].z, m.row[0].w, m.row[1].w, m.row[2].w, m.row[3].w);
}

Vector3 Matrix::RotationToEuler(Matrix m)
{
	// Source: https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2012/07/euler-angles1.pdf
	float theta1 = atan2f(m.row[1].z, m.row[2].z);
	float c2 = sqrt(m.row[0].x*m.row[0].x + m.row[0].y*m.row[0].y);
	float theta2 = atan2f(-m.row[0].z, c2);
	float c1 = cosf(theta1);
	float s1 = sinf(theta1);
	float theta3 = atan2f(s1*m.row[2].x - c1*m.row[1].x, c1*m.row[1].y -s1*m.row[2].y);
	return Vector3(theta1, theta2, theta3);
}

float Matrix::DeterminantHelper(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33)const
{
	float d11 = row[1].y*row[2].z - row[1].z*row[2].y;
	float d12 = row[1].x*row[2].z - row[1].z*row[2].x;
	float d13 = row[1].x*row[2].y - row[1].y*row[2].x;

	return row[0].x*d11 - row[0].y*d12 + row[0].z*d13;
}

NS_MATH_END