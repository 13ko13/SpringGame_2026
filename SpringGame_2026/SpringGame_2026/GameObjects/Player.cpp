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

	//プレイヤーの向きを変えるときの最低限のvelocityの大きさ
	constexpr float min_velocity_for_rot = 0.4f;

	//左スティックで受け取った値が大きくなりすぎるのを防ぐための上限と下限
	constexpr float input_value = 250.0f;

	//アニメーションの名前
	const char* const idle_anim_name = "Armature|Idle";
}

Player::Player(int modelHandle):
	GameObject(modelHandle, first_pos),
	m_animator(modelHandle)
{
	
}

Player::~Player()
{

}

void Player::Update()
{
	
}

void Player::Update(Input input , float angle)
{
	Matrix4x4 rotMtx;

	//値が大きすぎるため、上限と下限をあらかじめ設定する
	m_velocity.m_x = static_cast<float>(input.GetBufX()) / input_value;
	m_velocity.m_y = 0.0f;
	m_velocity.m_z = -static_cast<float>(input.GetBufY()) / input_value;

	//カメラの回転行列をプレイヤーの移動ベクトルにかけることで、カメラの向きに応じた移動ベクトルを計算する
	Matrix4x4 camRot = Matrix4x4::RotationY(angle);
	m_velocity = camRot.Transform(m_velocity);
	m_velocity.m_y = 0.0f;//プレイヤーは地面を移動するだけなので、Yの値は0にしてあげる

	//moveに入った値を正規化する
	Vector3 normVel = m_velocity.Normalized();

	if (normVel.Length() > min_velocity_for_rot)
	{
		float angleY = atan2f(normVel.m_x, -normVel.m_z);
		rotMtx = Matrix4x4::RotationY(angleY);
		m_prevAngleY = angleY;
	}
	else
	{
		rotMtx = Matrix4x4::RotationY(m_prevAngleY);
	}

	//正規化されたmoveに速度をかけたものをポジションに足してあげる
	m_pos += normVel * move_speed;

	//平行移動行列を作成
	Matrix4x4 transMtx = Matrix4x4::Matrix4x4::Translate(m_pos); 
	//拡大縮小行列を作成
	Matrix4x4 scaleMtx = Matrix4x4::Scale(model_size);
	//回転行列と平行移動行列を合成した行列を作成
	Matrix4x4 matrix = scaleMtx * rotMtx * transMtx;
	
	//合成した行列をモデルにセットする
	MV1SetMatrix(m_modelHandle, matrix.ToDxLib());

	if (CheckHitKey(KEY_INPUT_1))
	{
		//TODO:再生するアニメーション番号を引数に入れて、それを再生する
		m_animator.Play(MV1GetAnimIndex(m_modelHandle, idle_anim_name),10);
	}

	//アニメーションの更新
	m_animator.Update(0.1f);

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
	return m_pos + player_to_target;
}
