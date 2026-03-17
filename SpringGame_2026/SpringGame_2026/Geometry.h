#pragma once

/// <summary>
/// 3Dベクトルクラス
/// </summary>
struct Vector3
{
	float x, y, z;
	float Length()const;//ベクトルの大きさ
	void Normalize();//ベクトルの大きさを1にする
	Vector3 Normalized() const;///大きさを１にしたベクトルを返す
	Vector3 operator-()const;///ベクトルを反転したベクトルを返す
	//演算子オーバーロード
	void operator+=(const Vector3& val);///加算
	void operator-=(const Vector3& val);///減算
	void operator*=(const float scale);///ベクトルをn倍する
	Vector3 operator+(const Vector3& val)const;///加算
	Vector3 operator-(const Vector3& val)const;///減算
	Vector3 operator*(float scale)const;///ベクトルの乗算
	bool operator!=(const Vector3& val)const;///不等価比較
	bool operator==(const Vector3& val)const;///等価比較
};

