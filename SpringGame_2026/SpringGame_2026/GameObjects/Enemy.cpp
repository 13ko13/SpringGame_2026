#include <cmath>

#include "Enemy.h"
#include "../Math/Matrix4x4.h"
#include "../Manager/EffectManager.h"
#include "../Manager/SoundManager.h"

namespace
{
	const Vector3 model_size = { 1.5f,1.5f,1.5f };	//モデルのサイズ

	//アニメーションの名前
	const char* const idle_anim_name = "Armature|Idle";//待機
	const char* const hit_anim_name = "Armature|Hit";//攻撃を受けたとき
	const char* const dead_anim_name = "Armature|Dead";//死んでいるとき
	const char* const walk_anim_name = "Armature|Walk";//歩いているとき

	//アニメーションブレンドの速度
	constexpr float blend_time = 20.0f;

	//球の半径
	constexpr float sphere_r = 70.0f;

	//足元から自分の当たり判定の球までの距離
	const Vector3 enemy_to_sphere = { 0.0f,125.0f,0.0f };

	//Lerpに使うtの値
	constexpr float lerp_t = 0.01f;

	//敵がプレイヤーに向かって移動するときの速度
	constexpr float move_speed = 0.5f;

	//敵が移動するときの最低限のvelocityの大きさ
	constexpr float min_velocity = 0.1f;

	//歩きアニメーションの再生速度
	constexpr float walk_anim_speed = 1.5f;

	//敵の死亡アニメーションの倒れている状態の時間
	constexpr float dead_anim_time = 0.7f;
}

Enemy::Enemy(const int modelHandle, const Vector3& pos, std::shared_ptr<EffectManager> pManager) :
	GameObject(modelHandle, pos),
	m_animator(modelHandle),
	m_pEffectManager(pManager)
{
	//平行移動行列を作成
	Matrix4x4 transMtx = Matrix4x4::Translate(m_pos);
	//拡大縮小行列を作成
	Matrix4x4 scaleMtx = Matrix4x4::Scale(model_size);
	//平行移動行列と拡大縮小行列を合成した行列を作成
	Matrix4x4 mtx = scaleMtx * transMtx;

	//モデルに拡大縮小行列をかける
	MV1SetMatrix(m_modelHandle, mtx.ToDxLib());

	//敵のアニメーションが同じタイミングで始まるのを防ぐために、アニメーションの開始フレームをランダムにする
	//アニメーションの総再生時間を取得する
	float totalTime = MV1GetAnimTotalTime(m_modelHandle, MV1GetAnimIndex(m_modelHandle, idle_anim_name));
	//0からtotalTimeまでのランダムな値を生成する
	//RAND_MAXはrand()が返すことのできる最大値
	float startFrame = static_cast<float>(rand()) / RAND_MAX * totalTime;

	//初期アニメーションはidleにする
	m_animator.Play(MV1GetAnimIndex(m_modelHandle, idle_anim_name), true, 1.0f, startFrame);
}

Enemy::~Enemy()
{
	//特になし
}

void Enemy::Update()
{
	//if (m_currentState == State::Hit)
	//{
	//	if (m_animator.IsEnd())
	//	{
	//		//攻撃を受けたときのアニメーションが終わったら待機にする
	//		ChangeState(State::Idle);
	//	}
	//}

	//アニメーションを更新する
	m_animator.Update(blend_time);

	//Deadアニメーションが終わったらisDeadをtrueにする
	if (m_currentState == State::Dead)
	{
		//アニメーションの再生時間と総再生時間を取得する
		float animLength = m_animator.GetAnimLength();
		float playTime = m_animator.GetPlayTime();

		//アニメーションの再生率を求める
		float rate = playTime / animLength;
		if (rate >= dead_anim_time && !m_isPlayDeadSE)
		{
			//倒れるときの効果音を出す
			SoundManager::GetInstance().Play(SoundManager::SoundType::DeadEnemy);
			m_isPlayDeadSE = true;
		}

		if (m_animator.IsEnd())
		{
			m_isDead = true;
		}
	}

	//当たり判定の更新
	m_sphere.Update(m_pos + enemy_to_sphere, sphere_r);
}

void Enemy::Update(const Vector3& playerPos, const Vector3& stageSize, bool isCanMove)
{
	//基本的な更新処理
	Update();

	//移動が許可されていないなら
	//移動ベクトルを0にして処理を飛ばす
	if (isCanMove == false)
	{
		m_velocity = { 0.0f,0.0f,0.0f };
		return;
	}

	if (m_currentState != State::Dead)
	{
		//プレイヤーの方を線形補完して向くようにする
		//まずは敵からプレイヤーへのベクトルを求める
		Vector3 toPlayer = playerPos - m_pos;
		//Y成分はいらないので0にする
		toPlayer.m_y = 0.0f;

		//ベクトルの正規化
		Vector3 toPlayerNorm = toPlayer.Normalized();

		//モデルの初期の向き(-Z方向)とtoPlayerNormのなす角を求める
		float angleY = atan2f(toPlayerNorm.m_x, -toPlayerNorm.m_z);

		//ここからは角度の線形補完
		//現在の角度と求めた角度の差を求める
		float angleDiff = angleY - m_currentAngleY;
		//-π～πの範囲に収める
		// 角度の差が-πより小さい場合は360度回転させる
		while (angleDiff < -DX_PI_F) angleDiff += DX_TWO_PI_F;
		// 角度の差がπより大きい場合は360度回転させる
		while (angleDiff > DX_PI_F) angleDiff -= DX_TWO_PI_F;
		//現在の角度に角度の差のlerp_t倍を足す
		m_currentAngleY += angleDiff * lerp_t;

		//プレイヤーに向かって移動する
		m_velocity = toPlayerNorm * move_speed;
		m_pos += m_velocity;

		//拡大縮小行列を作成
		Matrix4x4 scaleMtx = Matrix4x4::Scale(model_size);
		//平行移動行列を作成
		Matrix4x4 transMtx = Matrix4x4::Translate(m_pos);
		//回転行列を作成
		//プレイヤーの方を向くようにする
		Matrix4x4 rotMtx = Matrix4x4::Matrix4x4::RotationY(m_currentAngleY);
		//拡大縮小行列と平行移動行列を合成した行列を作成
		Matrix4x4 mtx = scaleMtx * rotMtx;
		mtx = mtx * transMtx;

		//行列をモデルにセットする
		MV1SetMatrix(m_modelHandle, mtx.ToDxLib());

		if (IsMoving())
		{
			//移動しているときはWalkアニメーションにする
			ChangeState(State::Walk);

		}
		else
		{
			//止まっているときはIdleアニメーションにする
			ChangeState(State::Idle);
		}
	}

	//ステージから出れないようにする
	if (m_pos.m_x < -stageSize.m_x) m_pos.m_x = -stageSize.m_x;
	if (m_pos.m_z > stageSize.m_z) m_pos.m_z = stageSize.m_z;
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

	//死亡SEを鳴らすときのフラグをリセットする
	m_isPlayDeadSE = false;

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

	//敵のアニメーションが同じタイミングで始まるのを防ぐために、アニメーションの開始フレームをランダムにする
	//アニメーションの総再生時間を取得する
	float totalTime = MV1GetAnimTotalTime(m_modelHandle, MV1GetAnimIndex(m_modelHandle, idle_anim_name));
	//0からtotalTimeまでのランダムな値を生成する
	//RAND_MAXはrand()が返すことのできる最大値
	float startFrame = static_cast<float>(rand()) / RAND_MAX * totalTime;

	//ステートに応じてアニメーションを変更
	switch (m_currentState)
	{
	case State::Idle:
		m_animator.Play(MV1GetAnimIndex(m_modelHandle, idle_anim_name), true, 1.0f, startFrame);
		break;
	case State::Hit:
		m_animator.Play(MV1GetAnimIndex(m_modelHandle, hit_anim_name), false, 1.0f);
		break;
	case State::Dead:
		m_animator.Play(MV1GetAnimIndex(m_modelHandle, dead_anim_name), false, 1.0f);
		break;
	case State::Walk:
		m_animator.Play(MV1GetAnimIndex(m_modelHandle, walk_anim_name), true, walk_anim_speed, startFrame);
		break;
	}
}

bool const Enemy::IsMoving() const
{
	Vector3 v = m_velocity;
	v.m_y = 0.0f;//Y成分は無視する
	if (v.Length() > min_velocity)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Enemy::IsDeadAnimEnd() const
{
	//死亡アニメーションに入っていて、
	//まだアニメションが終わっていないときはfalseを返す
	if (m_currentState == State::Dead)
	{
		if (!m_animator.IsEnd())
		{
			return false;
		}
		else
		{
			//死んでいるときはアニメーションが終わっているかを返す
			return true;
		}
	}

	//死亡状態でない（生きている）ときはtrueを返す
	return true;
}
