#include "Matrix4x4.h"
#include "Vector3.h"
#include <cmath>

Matrix4x4::Matrix4x4() :
	m_x0(0.0f), m_x1(0.0f), m_x2(0.0f), m_x3(0.0f),
	m_y0(0.0f), m_y1(0.0f), m_y2(0.0f), m_y3(0.0f),
	m_z0(0.0f), m_z1(0.0f), m_z2(0.0f), m_z3(0.0f),
	m_w0(0.0f), m_w1(0.0f), m_w2(0.0f), m_w3(0.0f)
{
}

Matrix4x4::Matrix4x4(
	float x0, float x1, float x2, float x3,
	float y0, float y1, float y2, float y3,
	float z0, float z1, float z2, float z3,
	float w0, float w1, float w2, float w3) :
	m_x0(x0), m_x1(x1), m_x2(x2), m_x3(x3),
	m_y0(y0), m_y1(y1), m_y2(y2), m_y3(y3),
	m_z0(z0), m_z1(z1), m_z2(z2), m_z3(z3),
	m_w0(w0), m_w1(w1), m_w2(w2), m_w3(w3)
{
}

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& right) const
{
	return Matrix4x4(
		m_x0 + right.m_x0, m_x1 + right.m_x1, m_x2 + right.m_x2, m_x3 + right.m_x3,
		m_y0 + right.m_y0, m_y1 + right.m_y1, m_y2 + right.m_y2, m_y3 + right.m_y3,
		m_z0 + right.m_z0, m_z1 + right.m_z1, m_z2 + right.m_z2, m_z3 + right.m_z3,
		m_w0 + right.m_w0, m_w1 + right.m_w1, m_w2 + right.m_w2, m_w3 + right.m_w3
	);
}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4& right) const
{
	return Matrix4x4(
		m_x0 - right.m_x0, m_x1 - right.m_x1, m_x2 - right.m_x2, m_x3 - right.m_x3,
		m_y0 - right.m_y0, m_y1 - right.m_y1, m_y2 - right.m_y2, m_y3 - right.m_y3,
		m_z0 - right.m_z0, m_z1 - right.m_z1, m_z2 - right.m_z2, m_z3 - right.m_z3,
		m_w0 - right.m_w0, m_w1 - right.m_w1, m_w2 - right.m_w2, m_w3 - right.m_w3
	);
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& right) const
{
	return Matrix4x4(
		//X
		m_x0 * right.m_x0 + m_x1 * right.m_y0 + m_x2 * right.m_z0 + m_x3 * right.m_w0,
		m_x1 * right.m_x0 + m_x1 * right.m_y0 + m_x2 * right.m_z0 + m_x3 * right.m_w0,
		m_x2 * right.m_x0 + m_x1 * right.m_y0 + m_x2 * right.m_z0 + m_x3 * right.m_w0,
		m_x3 * right.m_x0 + m_x1 * right.m_y0 + m_x2 * right.m_z0 + m_x3 * right.m_w0,

		//Y
		m_y0 * right.m_x0 + m_y1 * right.m_y0 + m_y2 * right.m_z0 + m_y3 * right.m_w0,
		m_y1 * right.m_x0 + m_y1 * right.m_y0 + m_y2 * right.m_z0 + m_y3 * right.m_w0,
		m_y2 * right.m_x0 + m_y1 * right.m_y0 + m_y2 * right.m_z0 + m_y3 * right.m_w0,
		m_x3 * right.m_x0 + m_y1 * right.m_y0 + m_y2 * right.m_z0 + m_y3 * right.m_w0,

		//Z
		m_z0 * right.m_x0 + m_z1 * right.m_y0 + m_z2 * right.m_z0 + m_z3 * right.m_w0,
		m_z1 * right.m_x0 + m_z1 * right.m_y0 + m_z2 * right.m_z0 + m_z3 * right.m_w0,
		m_z2 * right.m_x0 + m_z1 * right.m_y0 + m_z2 * right.m_z0 + m_z3 * right.m_w0,
		m_z3 * right.m_x0 + m_z1 * right.m_y0 + m_z2 * right.m_z0 + m_z3 * right.m_w0,

		//W
		m_w0 * right.m_x0 + m_w1 * right.m_y0 + m_w2 * right.m_z0 + m_w3 * right.m_w0,
		m_w1 * right.m_x0 + m_w1 * right.m_y0 + m_w2 * right.m_z0 + m_w3 * right.m_w0,
		m_w2 * right.m_x0 + m_w1 * right.m_y0 + m_w2 * right.m_z0 + m_w3 * right.m_w0,
		m_w3 * right.m_x0 + m_w1 * right.m_y0 + m_w2 * right.m_z0 + m_w3 * right.m_w0
	);
}

Matrix4x4 Matrix4x4::RotationX(float angle)
{
	return Matrix4x4(
		1.0f,        0.0f,         0.0f, 0.0f,
		0.0f, cosf(angle), -sinf(angle), 0.0f,
		0.0f, sinf(angle),  cosf(angle), 0.0f,
		0.0f,        0.0f,         0.0f, 1.0f
	);
}

Matrix4x4 Matrix4x4::RotationY(float angle)
{
	return Matrix4x4(
		 cosf(angle),0.0f,sinf(angle),0.0f,
		        0.0f,1.0f,       0.0f,0.0f,
		-sinf(angle),0.0f,cosf(angle),0.0f,
		        0.0f,0.0f,       0.0f,1.0f
	);
}

Matrix4x4 Matrix4x4::RotationZ(float angle)
{
	return Matrix4x4(
		cosf(angle), -sinf(angle), 0.0f, 0.0f,
		sinf(angle),  cosf(angle), 0.0f, 0.0f,
		       0.0f,         0.0f, 1.0f, 0.0f,
		       0.0f,         0.0f, 0.0f, 1.0f
	);
}

Matrix4x4 Matrix4x4::Translate(Vector3 right)
{
	return Matrix4x4(
		     1.0f,      0.0f,      0.0f, 0.0f,
		     0.0f,      1.0f,      0.0f, 0.0f,
		     0.0f,      0.0f,      1.0f, 0.0f,
		right.m_x, right.m_y, right.m_z, 1.0f
	);
}

Matrix4x4 Matrix4x4::Scale(Vector3 right)
{
	return Matrix4x4(
		right.m_x,     0.0f,     0.0f, 0.0f,
		     0.0f,right.m_y,     0.0f, 0.0f,
		     0.0f,     0.0f,right.m_z, 0.0f,
		     0.0f,     0.0f,     0.0f, 1.0f
	);
}

Vector3 Matrix4x4::Transform(const Vector3& vector) const
{
	Vector3 result;
	
	result.m_x = vector.m_x * m_x0 + vector.m_y * m_y0 + vector.m_z * m_z0 + m_w0;
	result.m_y = vector.m_x * m_x1 + vector.m_y * m_y1 + vector.m_z * m_z1 + m_w1;
	result.m_z = vector.m_x * m_x2 + vector.m_y * m_y2 + vector.m_z * m_z2 + m_w2;

	return result;
}
