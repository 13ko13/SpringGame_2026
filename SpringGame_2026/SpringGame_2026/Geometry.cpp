#include "Geometry.h"
#include <DxLib.h>
#include <cmath>
/// <summary>
/// ベクトルの大きさを計算する
/// </summary>
/// <returns>ベクトルの大きさ</returns>
float Vector3::Length() const
{
	return std::hypot(x, y, z);
}


void Vector3::Normalize()
{
	auto len = Length();
	if (len == 0.0f)
	{
		//0除算を避ける
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		return;
	}

	//xとyをzlenで割ることで正規化する
	x /= len;
	y /= len;
	z /= len;
}

Vector3 Vector3::Normalized() const
{
	auto len = Length();
	if (len == 0.0f)
	{
		//0除算を避ける
		return { 0.0f,0.0f,0.0f };
	}
	return { x / len, y / len,z / len };
}

Vector3 Vector3::operator-() const
{
	/// ベクトルを反転したベクトルを返す
	return { -x,-y ,-z };
}

void Vector3::operator+=(const Vector3& val)
{
	/// ベクトルの加算
	x += val.x;
	y += val.y;
	z += val.z;
}

void Vector3::operator-=(const Vector3& val)
{
	/// ベクトルの減算
	x -= val.x;
	y -= val.y;
	z -= val.z;
}

void Vector3::operator*=(const float scale)
{
	/// ベクトルのスカラー倍
	x *= scale;
	y *= scale;
	z *= scale;
}

Vector3 Vector3::operator+(const Vector3& val) const
{
	/// ベクトルの加算
	return { x + val.x, y + val.y,z + val.z };
}

Vector3 Vector3::operator-(const Vector3& val) const
{
	/// ベクトルの減算
	return { x - val.x, y - val.y,z - val.z };
}

Vector3 Vector3::operator*(float scale) const
{
	/// ベクトルのスカラー倍
	return { x * scale, y * scale,z * scale };
}

bool Vector3::operator!=(const Vector3& val) const
{
	/// ベクトルの不等価比較
	return (x != val.x || y != val.y || z != val.z);
}

bool Vector3::operator==(const Vector3& val) const
{
	return (x == val.x && y == val.y && z == val.z);
}
