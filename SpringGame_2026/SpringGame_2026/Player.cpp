#include "Player.h"
#include <DxLib.h>

namespace
{
	constexpr Vector3 first_pos = { 0.0f,0.0f,0.0f };
	constexpr Vector3 player_size = { 1.5f,1.5f,1.5f  };
}

Player::Player(int modelHandle):
	GameObject(modelHandle, first_pos)
{
	//プレイヤーのサイズを変更する
	MV1SetScale(m_modelHandle, player_size.ToDxLib());
}

Player::~Player()
{

}

void Player::Update()
{
	//プレイヤーの位置をDxLibのVECTOR型に変換してMV1SetPosition関数に渡す
	MV1SetPosition(m_modelHandle,m_pos.ToDxLib());
}

void Player::Draw()
{
	//モデルを描画する
	MV1DrawModel(m_modelHandle);
}
