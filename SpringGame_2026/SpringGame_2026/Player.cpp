#include "Player.h"
#include <DxLib.h>

namespace
{
	constexpr Vector3 first_pos = { 0.0f,0.0f,0.0f };
}

Player::Player(int modelHandle):
	GameObject(m_modelHandle, first_pos)
{
	
}

Player::~Player()
{

}

void Player::Update()
{
	MV1SetPosition(m_modelHandle, VGet(m_pos.x, m_pos.y, m_pos.z));
}

void Player::Draw()
{
	MV1DrawModel(m_modelHandle);
}
