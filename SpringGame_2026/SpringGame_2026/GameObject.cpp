#include "GameObject.h"
#include <DxLib.h>

namespace
{
	constexpr float gravity_scale = 1.0f;
}

GameObject::GameObject(int modelHandle, Vector3 pos):
	m_pos(pos),
	m_modelHandle(modelHandle),
	m_velocity(0.0f,0.0f,0.0f),
	m_frame(0),
	m_gravity(gravity_scale)
{

}

GameObject::~GameObject()
{
}
