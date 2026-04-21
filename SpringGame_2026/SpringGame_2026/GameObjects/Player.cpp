#include "Player.h"
#include <DxLib.h>
#include "../System/Input.h"
#include "../Math/Matrix4x4.h"
#include "../Manager/EffectManager.h"
#include "../Manager/SoundManager.h"

namespace
{
	const Vector3 first_pos = { 0.0f,0.0f,0.0f };	//初期座標
	const Vector3 model_size = { 1.5f,1.5f,1.5f };	//モデルのサイズ

	constexpr float move_speed = 4.0f;				//移動速度

	//プレイヤー基準位置から注視点までのベクトル
	const Vector3 player_to_target = { 0.0f, 290.0f, 0.0f };

	//プレイヤーの向きを変えるときの最低限のvelocityの大きさ
	constexpr float min_velocity_for_rot = 0.4f;

	//左スティックで受け取った値が大きくなりすぎるのを防ぐための上限と下限
	constexpr float input_value = 250.0f;

	//アニメーションの名前
	const char* const idle_anim_name = "Armature|Idle";//待機
	const char* const run_anim_name = "Armature|Run";//移動
	const char* const jump_attack = "Armature|JumpAttack";//ジャンプ攻撃
	const char* const damage_anim_name = "Armature|Damage";//ダメージ

	//アニメーションのブレンド時間
	constexpr float anim_blend_time = 20.0f;

	//ジャンプ攻撃のアニメーションの速度
	constexpr float jump_attack_anim_speed = 1.4f;
	//ダメージを受けたときのアニメーションの速度
	constexpr float damage_anim_speed = 1.5f;

	//球の半径
	constexpr float sphere_r = 70.0f;
	constexpr float attack_sphere_r = 500.0f;

	//プレイヤーから自分の当たり判定の球までの距離
	const Vector3 player_to_sphere = { 0.0f,125.0f,0.0f };

	//プレイヤーのジャンプ攻撃が当たり始める時間
	constexpr float jump_attack_hit_start = 0.5f;
	//プレイヤーのジャンプ攻撃が当たり終わる時間
	constexpr float jump_attack_hit_end = 0.6f;

	//プレイヤーがダメージを受けて吹き飛んで地面につくときの時間
	constexpr float damage_fly_time = 0.3f;

	//Lerpに使うtの値
	constexpr float lerp_t = 0.15f;

	//無敵になる時間
	constexpr int invincible_time = 90;//1.5秒

	//エフェクトのオフセット
	const Vector3 effect_offset = { 0.0f, -50.0f, 0.0f };

	//ダメージ時のノックバックの強さ
	constexpr float knock_back_power = 5.0f;
}

Player::Player(int modelHandle, std::shared_ptr<EffectManager> pManager) :
	GameObject(modelHandle, first_pos),
	m_animator(modelHandle),
	m_attackSphere(m_pos),
	m_pEffectManager(pManager)
{
	//再生するアニメーション番号を引数に入れて、それを再生する
	//初期状態はIdleにする
	m_animator.Play(MV1GetAnimIndex(m_modelHandle, idle_anim_name), true);

	//拡大縮小行列を作成
	Matrix4x4 mtx = Matrix4x4::Scale(model_size);
	//モデルに拡大縮小行列をかける
	MV1SetMatrix(m_modelHandle, mtx.ToDxLib());
}

Player::~Player()
{

}

void Player::Update()
{

}

void Player::Update(Input input, float angle, const Vector3& stageSize, bool isCanMove)
{
	//シーン側が移動を許可していないときは、
	//移動アニメーションを行いたくないため移動ベクトルを0にする
	if (isCanMove == false)
	{
		m_velocity = { 0.0f,0.0f,0.0f };
	}

	//シーン側がプレイヤーの移動を許可しているかどうかを受け取る
	m_isCanMove = isCanMove;

	//当たり判定球の更新
	UpdateCollSphere();

	//移動中に走っているときの音を鳴らす
	if (m_currentState == State::Move)
	{
		//走っているときの音を鳴らす
		SoundManager::GetInstance().Play(SoundManager::SoundType::RunPlayer,true);
	}
	else
	{
		//走っているときの音を止める
		SoundManager::GetInstance().Stop(SoundManager::SoundType::RunPlayer);
	}

	//攻撃中
	if (m_currentState == State::Attack)
	{
		//攻撃が当たる時間帯の時だけ、攻撃判定用の球を有効にする
		float time = m_animator.GetPlayTime();//現在のアニメーションの再生時間
		float length = m_animator.GetAnimLength();//現在のアニメーションの総再生時間
		float rate = time / length;//アニメーションの再生率(0.0~1.0)

		//ジャンプ攻撃が当たっている時間帯なら
		if (rate >= jump_attack_hit_start && rate <= jump_attack_hit_end)
		{
			m_isAttacking = true;//攻撃が当たる時間帯のときは攻撃中フラグを立てる
			m_attackSphereR = attack_sphere_r;//攻撃判定用の球の半径を大きくする
		}
		else
		{
			//それ以外なら
			m_isAttacking = false;//フラグを降ろす
			m_attackSphereR = 0.0f;//攻撃判定用の球の半径を0にする
		}

		//ジャンプ攻撃の着地時に着地音を鳴らす
		if (rate >= jump_attack_hit_end && !m_isPlayLandSE)
		{
			//着地音を鳴らす
			SoundManager::GetInstance().Play(SoundManager::SoundType::OnGround);
			m_isPlayLandSE = true;
		}

		//攻撃アニメーションがおわるまで　
		//他の行動はできないようにする
		if (m_animator.IsEnd())
		{
			//攻撃アニメーションが終わったら、攻撃中フラグを下ろす
			m_isAttacking = false;
			m_isCanMove = true;

			//入力を読み取って移動ベクトルを更新する
			//（Attack→Move の直接遷移でブレンドを正しくするため）
			Move(input, angle);

			//終わったらステートを攻撃入力されていたら
			//Attackにする
			//移動中ならMove,
			//何もしていないならIdleに切り替える
			//攻撃入力を最優先にする
			if (input.IsPressed("attack"))
			{
				Attack();
				//攻撃入力がされたときに、攻撃エフェクトを出す
				m_pEffectManager->Create(m_pos + effect_offset, EffectManager::EffectType::AttackField);
			}
			else if (IsMoving())
			{
				ChangeState(State::Move);
			}
			else
			{
				ChangeState(State::Idle);
			}
		}
		else
		{
			//攻撃中は移動ベクトルをリセットする
			m_velocity = { 0.0f,0.0f,0.0f };
		}

		m_animator.Update(anim_blend_time);
		//処理を抜ける
		return;
	}

	if (m_currentState != State::Damage)
	{
		//動ける状態のみ
		if (m_isCanMove)
		{
			//移動
			Move(input, angle);
		}

		//攻撃入力を最優先にする
		if (input.IsPressed("attack"))
		{
			//動けない場合は処理を飛ばす
			if (m_isCanMove == false) return;

			Attack();
			//攻撃入力がされたときに、攻撃エフェクトを出す
			m_pEffectManager->Create(m_pos + effect_offset, EffectManager::EffectType::AttackField);
		}
		else
		{
			//攻撃入力がされていないときは、攻撃判定用の球の半径を小さくする
			m_attackSphereR = 0.0f;

			//攻撃が押されていない場合で、
			//移動中であれば、ステートを移動にする
			if (IsMoving())
			{
				ChangeState(State::Move);
			}
			else
			{
				//全てに当てはまらない場合待機にする
				ChangeState(State::Idle);
			}
		}
	}
	else
	{
		//Damageアニメーションが終わったら移動中ならMove、それ以外はIdleに戻す
		if (m_animator.IsEnd())
		{
			m_isCanMove = true;
			if (IsMoving())
			{
				ChangeState(State::Move);
			}
			else
			{
				ChangeState(State::Idle);
			}
		}
	}

	//ダメージを受けた時のみに無敵フラグを立てる
	if (m_currentState == State::Damage)
	{
		m_isInvincible = true;

		//プレイヤーが地面に当たる時間帯になったら、ノックバックの速度を0にする
		float time = m_animator.GetPlayTime();//現在のアニメーションの再生時間
		float length = m_animator.GetAnimLength();//現在のアニメーションの総再生時間
		float rate = time / length;//アニメーションの再生率(0.0~1.0)

		//地面に当たっている時間帯なら
		if (rate >= damage_fly_time)
		{
			//地面に当たっている時間帯のときはノックバックの速度を0にする
			m_knockBackVelocity = { 0.0f,0.0f,0.0f };
		}

		//ダメージを受けたときのノックバックの速度を入れる
		m_pos += m_knockBackVelocity;

		//拡大縮小行列を作成
		Matrix4x4 scaleMtx = Matrix4x4::Scale(model_size);
		//回転行列を作成
		Matrix4x4 rotMtx = Matrix4x4::RotationY(m_currentAngleY);
		//平行移動行列を作成
		Matrix4x4 transMtx = Matrix4x4::Matrix4x4::Translate(m_pos);

		//回転行列と拡大縮小行列を合成した行列を作成
		Matrix4x4 matrix = scaleMtx * rotMtx;

		//さらにそれに平行移動行列をかけて、最終的な行列を作成する
		matrix = matrix * transMtx;

		//合成した行列をモデルにセットする
		MV1SetMatrix(m_modelHandle, matrix.ToDxLib());
	}

	//無敵状態のときは、無敵時間用のタイマーを進める
	if (m_isInvincible)
	{
		m_invincibleTimer++;
		//無敵時間が一定時間を超えたら、無敵状態を解除する
		if (m_invincibleTimer > invincible_time)
		{
			m_isInvincible = false;
			m_invincibleTimer = 0;
		}
	}

	//アニメーションの更新
	m_animator.Update(anim_blend_time);

#if _DEBUG

	//家ではキーボードしか使えないので
	//キーでも使える入力方法
	Vector3 dir = { 0.0f,0.0f,0.0f };//プレイヤーの方向ベクトル
	m_velocity.m_x = 0.0f;
	if (input.IsPressed("right"))
	{
		dir.m_x = 1.0f;
	}
	if (input.IsPressed("left"))
	{
		dir.m_x = -1.0f;
	}
	if (input.IsPressed("down"))
	{
		dir.m_z = -1.0f;
	}
	if (input.IsPressed("up"))
	{
		dir.m_z = 1.0f;
	}

	m_velocity = dir;
#endif

	//ステージから出れないようにする
	if (m_pos.m_x < -stageSize.m_x) m_pos.m_x = -stageSize.m_x;
	if (m_pos.m_x > stageSize.m_x) m_pos.m_x = stageSize.m_x;
	if (m_pos.m_z > stageSize.m_z) m_pos.m_z = stageSize.m_z;
	if (m_pos.m_z < -stageSize.m_z) m_pos.m_z = -stageSize.m_z;
}

void Player::Draw()
{
	//モデルを描画する
	MV1DrawModel(m_modelHandle);

#ifdef _DEBUG
	m_sphere.Draw(0xff0000);//当たり判定用の球を描画する
	m_attackSphere.Draw(0x0000ff);//攻撃判定用の球を描画する
#endif
}

Vector3 const Player::GetTargetPos() const
{
	//注視点の位置を渡す
	return m_pos + player_to_target;
}

void Player::Move(Input input, float angle)
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

	//移動ベクトルの大きさを求める
	float len = m_velocity.Length();

	if (len > min_velocity_for_rot)
	{
		Vector3 dir = m_velocity.Normalized();//移動ベクトルの向きを求める

		//atan2fで、移動ベクトルの向きから回転角を求める
		float angleY = atan2f(dir.m_x, -dir.m_z);

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

		//それをもとに回転行列を作成
		rotMtx = Matrix4x4::RotationY(m_currentAngleY);
		//前のフレームの回転角を更新
		m_prevAngleY = angleY;
	}
	else
	{
		rotMtx = Matrix4x4::RotationY(m_prevAngleY);
	}

	//posに移動ベクトルを足して次のフレームの位置を計算する
	m_pos += m_velocity * move_speed;

	//平行移動行列を作成
	Matrix4x4 transMtx = Matrix4x4::Matrix4x4::Translate(m_pos);
	//拡大縮小行列を作成
	Matrix4x4 scaleMtx = Matrix4x4::Scale(model_size);
	//回転行列と平行移動行列を合成した行列を作成
	Matrix4x4 matrix = scaleMtx * rotMtx * transMtx;

	//合成した行列をモデルにセットする
	MV1SetMatrix(m_modelHandle, matrix.ToDxLib());
}

void Player::Attack()
{
	//動けなくする
	m_isCanMove = false;
	//攻撃開始音を鳴らす
	SoundManager::GetInstance().Play(SoundManager::SoundType::AttackStart);
	m_isPlayLandSE = false;//着地音を鳴らせるようにする

	//ステートを攻撃中に変更
	ChangeState(State::Attack, true);
}

bool const Player::IsMoving() const
{
	Vector3 v = m_velocity;
	v.m_y = 0.0f;//Y成分は無視する
	if (v.Length() > min_velocity_for_rot)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Player::ChangeState(State next, bool force)
{
	//切り替えたいステートがすでに適用されている場合は
	//処理を飛ばす
	if (m_currentState == next &&
		!force) return;

	//ステートを切り替える
	m_currentState = next;

	//ステートに応じてアニメーションを変更
	switch (m_currentState)
	{
	case State::Idle:
		m_animator.Play(MV1GetAnimIndex(m_modelHandle, idle_anim_name), true);
		break;
	case State::Move:
		m_animator.Play(MV1GetAnimIndex(m_modelHandle, run_anim_name), true);
		break;
	case State::Attack:
		m_animator.Play(MV1GetAnimIndex(m_modelHandle, jump_attack), false, jump_attack_anim_speed);
		break;
	case State::Damage:
		m_animator.Play(MV1GetAnimIndex(m_modelHandle, damage_anim_name), false, damage_anim_speed);
		break;
	}
}

void Player::UpdateCollSphere()
{
	Vector3 spherePos = m_pos + player_to_sphere;//球の位置をプレイヤーの位置から少し上にする
	m_sphere.Update(spherePos, sphere_r);//当たり判定用の球を更新する

	//攻撃判定用の球も同様に更新する
	m_attackSphere.Update(m_pos, m_attackSphereR);
}

Vector3 Player::GetForward() const
{
	Vector3 forward;
	forward = {
		sinf(m_prevAngleY),
		0.0f,
		-cosf(m_prevAngleY)
	};
	return forward;
}

void Player::OnDamage()
{
	//プレイヤーが攻撃中または
	//無敵中なら処理を飛ばす
	//被弾中は処理を飛ばす
	if (m_currentState == State::Attack ||
		m_isInvincible || m_currentState == State::Damage) return;

	//後ろに吹き飛ぶ力を加える
	m_knockBackVelocity = -GetForward() * knock_back_power;

	//ダメージを受けたときは、攻撃判定用の球の半径を0にして、当たり判定をなくす
	m_attackSphereR = 0.0f;
	//移動できないようにする
	m_isCanMove = false;

	//音を鳴らす
	SoundManager::GetInstance().Play(SoundManager::SoundType::HitPlayer);

	//ステートをダメージにする
	ChangeState(State::Damage);
}

void Player::OnHitEnemy(const Vector3& enemyPos)
{
	//攻撃が当たった時の音
	SoundManager::GetInstance().Play(SoundManager::SoundType::Thunder);
	//エフェクトマネージャーにエフェクトの再生を依頼する
	m_pEffectManager->Create(enemyPos, EffectManager::EffectType::HitEnemy);
}
