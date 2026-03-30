#include "Player.h"
#include <DxLib.h>
#include "../System/Input.h"
#include "../Math/Matrix4x4.h"

namespace
{
	const Vector3 first_pos = { 0.0f,0.0f,0.0f };	//初期座標
	const Vector3 model_size = { 1.5f,1.5f,1.5f };	//モデルのサイズ

	constexpr float move_speed = 2.0f;				//移動速度

	//プレイヤー基準位置から注視点までのベクトル
	const Vector3 player_to_target = { 0.0f, 290.0f, 0.0f };

	//プレイヤーの向きを変えるときの最低限のvelocityの大きさ
	constexpr float min_velocity_for_rot = 0.4f;

	//左スティックで受け取った値が大きくなりすぎるのを防ぐための上限と下限
	constexpr float input_value = 250.0f;

	//アニメーションの名前
	const char* const idle_anim_name = "Armature|Idle";//待機
	const char* const run_anim_name = "Armature|Run";//移動
	const char* const punch_anim_name = "Armature|Punch";//パンチ

	//アニメーションのブレンド時間
	constexpr float anim_blend_time = 20.0f;

	//パンチのアニメーションの速度
	constexpr float punch_anim_speed = 1.8f;

	//球の半径
	constexpr float sphere_r = 100.0f;
	constexpr float attack_sphere_r = 70.0f;

	//プレイヤーから自分の当たり判定の球までの距離
	const Vector3 player_to_sphere = { 0.0f,125.0f,0.0f };

	//プレイヤーから攻撃判定用の球までの高さ(距離)
	const Vector3 player_to_attackSphere = { 0.0f, 150.0f, 0.0f };

	//プレイヤーのパンチが当たり始める時間
	constexpr float punch_hit_start = 0.4f;
	//プレイヤーのパンチが当たり終わる時間
	constexpr float punch_hit_end = 0.7f;
}

Player::Player(int modelHandle) :
	GameObject(modelHandle, first_pos),
	m_animator(modelHandle),
	m_attackSphere(m_pos + player_to_attackSphere)
{
	//再生するアニメーション番号を引数に入れて、それを再生する
	//初期状態はIdleにする
	m_animator.Play(MV1GetAnimIndex(m_modelHandle, idle_anim_name),true);
}

Player::~Player()
{

}

void Player::Update()
{

}

void Player::Update(Input input, float angle)
{
	UpdateCollSphere();

	//攻撃中
	if (m_currentState == State::Attack)
	{
		//攻撃が当たる時間帯の時だけ、攻撃判定用の球を有効にする
		float time = m_animator.GetPlayTime();//現在のアニメーションの再生時間
		float length = m_animator.GetAnimLength();//現在のアニメーションの総再生時間
		float rate = time / length;//アニメーションの再生率(0.0~1.0)

		//パンチが当たっている時間帯なら
		if (rate >= punch_hit_start && rate <= punch_hit_end)
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

		//移動ベクトルをリセットする
		m_velocity = { 0.0f,0.0f,0.0f };

		//攻撃アニメーションがおわるまで　
		//他の行動はできないようにする
		if (m_animator.IsEnd())
		{
			//攻撃アニメーションが終わったら、攻撃中フラグを下ろす
			m_isAttacking = false;

			//終わったらステートを攻撃入力されていたら
			//Attackにする
			//移動中ならMove,
			//何もしていないならIdleに切り替える
			//攻撃入力を最優先にする
			if (input.IsTriggered("attack"))
			{
				Attack();
			}
			else
			{
				if (IsMoving())
				{
					ChangeState(State::Move);
				}
				else
				{
					ChangeState(State::Idle);
				}
			}
			m_isCanMove = true;
		}

		m_animator.Update(anim_blend_time);
		//処理を抜ける
		return;
	}

	//動ける状態のみ
	if (m_isCanMove)
	{
		//移動
		Move(input, angle);
	}

	//攻撃入力を最優先にする
	if (input.IsTriggered("attack"))
	{
		Attack();
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


	//アニメーションの更新
	m_animator.Update(anim_blend_time);

#if _DEBUG

	//家ではキーボードしか使えないので
	//キーでも使える入力方法
	Vector3 dir = { 0.0f,0.0f,0.0f };//プレイヤーの速度ベクトル
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
}

void Player::Draw()
{
	//モデルを描画する
	MV1DrawModel(m_modelHandle);

#ifdef _DEBUG
	DrawFormatString(0, 30, 0xffffff, "PlayerState:%d", m_currentState);//ステートを画面に表示する
	m_sphere.Draw(0xff0000);//当たり判定用の球を描画する
	m_attackSphere.Draw(0x0000ff);//攻撃判定用の球を描画する
#endif
}

Vector3 const Player::GetTargetPos() const
{
	//注視点の位置を渡す
	return m_pos + player_to_target;
}

void Player::OnAttackedEnemy()
{
	//敵が何度もHitアニメーションをしないように
	//一度攻撃が当たったら、攻撃判定用の球の半径を0にして、当たり判定をなくす
	m_attackSphereR = 0.0f;

	m_isAttacking = false;
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
		//それをもとに回転行列を作成
		rotMtx = Matrix4x4::RotationY(angleY);
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

	//ステートを攻撃中に変更
	ChangeState(State::Attack);
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

void Player::ChangeState(State next)
{
	//切り替えたいステートがすでに適用されている場合は
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
	case State::Move:
		m_animator.Play(MV1GetAnimIndex(m_modelHandle, run_anim_name), true);
		break;
	case State::Attack:
		m_animator.Play(MV1GetAnimIndex(m_modelHandle, punch_anim_name), false, punch_anim_speed);
		break;
	}
}

void Player::UpdateCollSphere()
{
	Vector3 spherePos = m_pos + player_to_sphere;//球の位置をプレイヤーの位置から少し上にする
	m_sphere.Update(spherePos, sphere_r);//当たり判定用の球を更新する

	//攻撃判定用の球も同様に更新する
	m_attackSphere.Update(m_pos + GetForward() * 130.0f + player_to_attackSphere, m_attackSphereR);
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
