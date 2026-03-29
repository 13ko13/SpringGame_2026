#include "Sphere.h"

Sphere::Sphere(const Vector3& pos):
	m_pos(pos)
{

}

Sphere::~Sphere()
{
}

void Sphere::Update(const Vector3& pos, const float radius)
{
	m_pos = pos;
	m_radius = radius;
}

void Sphere::Draw(unsigned int color)
{
	DrawSphere3D(m_pos.ToDxLib(), m_radius, 16, color, color, FALSE);
}
