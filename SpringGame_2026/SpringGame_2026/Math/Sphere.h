#pragma once
#include "Vector3.h"

class Sphere
{
public:
	Sphere(const Vector3& pos);
	~Sphere();

	void Update(const Vector3& pos, const float radius);
	void Draw(unsigned int color);

private:
	//位置
	Vector3 m_pos;

	//半径
	float m_radius = 0.0f;
};

