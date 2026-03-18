#include "Player.h"
#include <DxLib.h>
#include "../System/Input.h"

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
	m_velocity.m_x = static_cast<float>(input.GetBufX()) / 250.0f;
	m_velocity.m_y = 0.0f;
	m_velocity.m_z = -static_cast<float>(input.GetBufY()) / 250.0f;

	//moveに入った値を正規化する
	Vector3 normMove = m_velocity.Normalized();
	//正規化されたmoveに速度をかけたものをポジションに足してあげる
	m_pos += normMove * move_speed;

	//プレイヤーの位置をDxLibのVECTOR型に変換してMV1SetPosition関数に渡す
	MV1SetPosition(m_modelHandle, m_pos.ToDxLib());

#if _DEBUG

	//家ではキーボードしか使えないので
	//キーでも使える入力方法
	Vector3 dir = { 0.0f,0.0f,0.0f};//プレイヤーの速度ベクトル
	m_velocity.m_x = 0.0f;
	if (input.IsPressed("right"))
	{
		dir.m_x = 1.0f;
	}
	if (input.IsPressed("left"))
	{
		dir.m_x = -1.0f;
	}
	if(input.IsPressed("down"))
	{
		dir.m_z = -1.0f;
	}
	if (input.IsPressed("up"))
	{
		dir.m_z = 1.0f;
	}

	//ディレクションを正規化してプレイヤーのスピードをかけて
	//ポジションに足してあげる移動処理
	m_velocity += dir.Normalized() * move_speed;

	m_pos += m_velocity;
#endif
}

void Player::Draw()
{
	//モデルを描画する
	MV1DrawModel(m_modelHandle);
}
