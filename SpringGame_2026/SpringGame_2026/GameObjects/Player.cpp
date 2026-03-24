#include "Player.h"
#include <DxLib.h>
#include "../System/Input.h"
#include "../Math/Matrix4x4.h"

namespace
{
	const Vector3 first_pos = { 0.0f,0.0f,0.0f };	//初期座標
	const Vector3 model_size = { 1.5f,1.5f,1.5f  };	//モデルのサイズ

	constexpr float move_speed = 6.0f;				//移動速度

	//プレイヤー基準位置から注視点までのベクトル
	const Vector3 player_to_target = { 0.0f, 290.0f, 0.0f };
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

void Player::Update(Input input , float angle)
{
	//カメラの正面ベクトルを取得して、それをもとにプレイヤーの移動ベクトルを計算する
	Matrix4x4 rotMtx = Matrix4x4::RotationY(angle);

	//値が大きすぎるため、上限と下限をあらかじめ設定する
	m_velocity.m_x = static_cast<float>(input.GetBufX()) / 250.0f;
	m_velocity.m_y = 0.0f;
	m_velocity.m_z = -static_cast<float>(input.GetBufY()) / 250.0f;

	//カメラの回転行列をプレイヤーの移動ベクトルにかけることで、カメラの向きに応じた移動ベクトルを計算する
	m_velocity = rotMtx.Transform(m_velocity);
	m_velocity.m_y = 0.0f;//プレイヤーは地面を移動するだけなので、Yの値は0にしてあげる

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

	m_velocity = dir;
#endif
}

void Player::Draw()
{
	//モデルを描画する
	MV1DrawModel(m_modelHandle);
}

Vector3 const Player::GetTargetPos() const
{
	//注視点の位置を渡す
	Vector3 pos = m_pos;
	pos.m_y = 0.0f;
	
	return pos + player_to_target;
}
