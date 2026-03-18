#include "Player.h"
#include <DxLib.h>
#include "Input.h"

namespace
{
	const Vector3 first_pos = { 0.0f,0.0f,0.0f };	//初期座標
	const Vector3 model_size = { 1.5f,1.5f,1.5f  };	//モデルのサイズ

	constexpr float move_speed = 4.0f;				//移動速度
}

Player::Player(int modelHandle):
	GameObject(modelHandle, first_pos)
{
	//プレイヤーのサイズを変更する
	MV1SetScale(m_modelHandle, model_size.ToDxLib());
}

Player::~Player()
{

}

void Player::Update()
{
	
}

void Player::Update(Input input)
{
	//値が大きすぎるため、上限と下限をあらかじめ設定する
	Vector3 move;
	move.m_x = static_cast<float>(input.GetBufX()) / 250.0f;
	move.m_y = 0.0f;
	move.m_z = -static_cast<float>(input.GetBufY()) / 250.0f;

	//moveに入った値を正規化する
	Vector3 normMove = move.Normalized();
	//正規化されたmoveに速度をかけたものをポジションに足してあげる
	m_pos += normMove * move_speed;

	//プレイヤーの位置をDxLibのVECTOR型に変換してMV1SetPosition関数に渡す
	MV1SetPosition(m_modelHandle, m_pos.ToDxLib());
}

void Player::Draw()
{
	//モデルを描画する
	MV1DrawModel(m_modelHandle);
}
