#pragma once

//前方宣言
class Vector3;

/// <summary>
/// 4x4行列クラス
/// </summary>
class Matrix4x4
{
public:
	Matrix4x4();

	Matrix4x4(float x0, float x1, float x2, float x3,
			  float y0, float y1, float y2, float y3,
			  float z0, float z1, float z2, float z3,
			  float w0, float w1, float w2, float w3  );

	//行列の足し算
	Matrix4x4 operator+(const Matrix4x4& right) const;

	//行列の引き算
	Matrix4x4 operator-(const Matrix4x4& right) const;

	//行列の掛け算
	Matrix4x4 operator*(const Matrix4x4& r) const;

	//X軸回転行列
	static Matrix4x4 RotationX(float angle);

	//Y軸回転行列
	static Matrix4x4 RotationY(float angle);

	//Z軸回転行列
	static Matrix4x4 RotationZ(float angle);

	//平行移動行列
	static Matrix4x4 Translate(Vector3 right);

	//拡大縮小行列
	static Matrix4x4 Scale(Vector3 right);

	//行列とベクトルの掛け算
	Vector3 Transform(const Vector3& vector) const;


private:
	float m_x0, m_x1, m_x2, m_x3,
		  m_y0, m_y1, m_y2, m_y3,
		  m_z0, m_z1, m_z2, m_z3,
		  m_w0, m_w1, m_w2, m_w3;
};

