#include "GameObject.h"
#include <DxLib.h>

namespace
{
	//重力
	constexpr float gravity_scale = 1.0f;
}

GameObject::GameObject(int modelHandle, const Vector3& pos):
	m_pos(pos),
	m_modelHandle(modelHandle),
	m_velocity(0.0f,0.0f,0.0f),
	m_frame(0),
	m_gravity(gravity_scale),
	m_sphere(pos)
{

}

GameObject::GameObject(const Vector3& pos):
	m_pos(pos),
	m_modelHandle(-1),
	m_velocity(0.0f, 0.0f, 0.0f),
	m_frame(0),
	m_gravity(gravity_scale),
	m_sphere(pos)
{
	
}

GameObject::~GameObject()
{

}
