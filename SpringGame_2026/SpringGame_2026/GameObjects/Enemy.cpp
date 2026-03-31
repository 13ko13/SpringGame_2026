#include "Enemy.h"
#include "../Math/Matrix4x4.h"

namespace
{
	const Vector3 model_size = { 1.5f,1.5f,1.5f };	//モデルのサイズ

	//アニメーションの名前
	const char* const idle_anim_name = "Armature|Idle";//待機
	const char* const hit_anim_name = "Armature|Hit";//攻撃を受けたとき
	const char* const dead_anim_name = "Armature|Dead";//死んでいるとき

	//アニメーションブレンドの速度
	constexpr float blend_time = 20.0f;

	//球の半径
	constexpr float sphere_r = 100.0f;

	//足元から自分の当たり判定の球までの距離
	const Vector3 enemy_to_sphere = { 0.0f,125.0f,0.0f };
}

Enemy::Enemy(const int modelHandle,const Vector3& pos):
	GameObject(modelHandle,pos),
	m_animator(modelHandle)
{
	MV1SetPosition(m_modelHandle, m_pos.ToDxLib());

	//初期アニメーションはidleにする
	m_animator.Play(MV1GetAnimIndex(m_modelHandle, idle_anim_name), true);
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	//拡大縮小行列を作成
	Matrix4x4 scaleMtx = Matrix4x4::Scale(model_size);
	//平行移動行列を作成
	Matrix4x4 transMtx = Matrix4x4::Translate(m_pos);
	//拡大縮小行列と平行移動行列を合成した行列を作成
	Matrix4x4 mtx = scaleMtx * transMtx;

	//行列をモデルにセットする
	MV1SetMatrix(m_modelHandle, mtx.ToDxLib());

	//if (m_currentState == State::Hit)
	//{
	//	if (m_animator.IsEnd())
	//	{
	//		//攻撃を受けたときのアニメーションが終わったら待機にする
	//		ChangeState(State::Idle);
	//	}
	//}

	//Deadアニメーションが終わったらisDeadをtrueにする
	if (m_currentState == State::Dead)
	{
		if (m_animator.IsEnd())
		{
			m_isDead = true;
		}
	}

	//アニメーションを更新する
	m_animator.Update(blend_time);

	//当たり判定の更新
	m_sphere.Update(m_pos + enemy_to_sphere, sphere_r);
}

void Enemy::Draw()
{
	//モデルを描画する
	MV1DrawModel(m_modelHandle);

#if _DEBUG
	//当たり判定用の球を描画する
	m_sphere.Draw(0xff0000);
#endif
}

void Enemy::OnHit()
{
	//既にステートがHitの場合は処理を飛ばす
	if (m_currentState == State::Hit) return;

	//ステートをHitに変更する
	ChangeState(State::Hit);
}

void Enemy::OnDead()
{
	//既にステートがDeadの場合は処理を飛ばす
	if (m_currentState == State::Dead) return;

	//ステートをDeadに変更する
	ChangeState(State::Dead);
}

void Enemy::ChangeState(State next)
{ 
	//切り替えたいステートが既に適用されていたら
	//処理を飛ばす
	if (m_currentState == next) return;

	//ステートを切り替える
	m_currentState = next;

	//ステートに応じてアニメーションを変更
	switch (m_currentState)
	{
	case State::Idle:
		m_animator.Play(MV1GetAnimIndex(m_modelHandle, idle_anim_name), true);
		break;
	case State::Hit:
		m_animator.Play(MV1GetAnimIndex(m_modelHandle, hit_anim_name), false);
		break;
	case State::Dead:
		m_animator.Play(MV1GetAnimIndex(m_modelHandle, dead_anim_name), false);
	}
}
