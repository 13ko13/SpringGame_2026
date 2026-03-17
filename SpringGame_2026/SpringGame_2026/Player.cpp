#include "Player.h"
#include <DxLib.h>

namespace
{
	constexpr Vector3 first_pos = { 0.0f,0.0f,0.0f };
}

Player::Player(int modelHandle):
	GameObject(m_modelHandle, first_pos,m_velocity)
{
	//TODO:モデルをロードする
	//m_modelHandle = MV1LoadModel();
}

Player::~Player()
{

}

void Player::Update()
{

}

void Player::Draw()
{
	
}
