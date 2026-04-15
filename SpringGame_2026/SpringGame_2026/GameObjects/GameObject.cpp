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
	//モデルのハンドルが有効な値ならモデルを削除する
	if (m_modelHandle >= 0)
	{
		MV1DeleteModel(m_modelHandle);
		m_modelHandle = -1;
	}
}

void GameObject::OnPushBack(const Vector3& pushVector)
{
	//押し戻しのベクトルを加算する
	m_pos += pushVector;
}
