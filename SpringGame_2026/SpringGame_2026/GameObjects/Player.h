#pragma once
#include "GameObject.h"
#include "../Math/Vector3.h"
#include "../System/ModelAnimator.h"
#include "../Math/Sphere.h"
#include <memory>

class Input;
class EffectManager;
class Camera;

class Player : public GameObject
{
public:
	Player(int modelHandle, std::shared_ptr<EffectManager> pManager, std::shared_ptr<Camera> pCamera);
	~Player();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 入力を受け取っている更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	/// <param name="angle">カメラの回転角</param>
	void Update(Input input, float angle, const Vector3& stageSizeis, bool isCanMove);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// カメラのターゲットにさせる位置を取得
	/// </summary>
	/// <returns>カメラのターゲットの位置</returns>
	Vector3 const GetTargetPos() const;

	/// <summary>
	/// 攻撃用の球を取得する
	/// </summary>
	/// <returns>攻撃用の球</returns>
	Sphere GetAttackSphere() const { return m_attackSphere; }

	/// <summary>
	/// 攻撃中かを返す
	/// </summary>
	/// <returns>true:攻撃中,false:攻撃中ではない</returns>
	bool IsAttacking() const { return m_isAttacking; }

	/// <summary>
	/// ダメージを受けたときの処理
	/// </summary>
	void OnDamage();

	/// <summary>
	/// 敵に攻撃が当たった時の処理
	/// </summary>
	/// <param name="enemyPos">敵の位置</param>
	/// <param name="hitEnemyNum">攻撃が当たった敵の数</param>
	void OnHitEnemy(const Vector3& enemyPos);

private:
	//無敵時間用のタイマー
	int m_invincibleTimer = 0;
	//無敵状態かどうか
	bool m_isInvincible = false;

	//ノックバック用の速度ベクトル
	Vector3 m_knockBackVelocity = { 0.0f,0.0f,0.0f };

	Vector3 m_targetPos = { 0.0f,0.0f,0.0f };//カメラに注視させる位置

	float m_prevAngleY = 0.0f;//前のフレームのプレイヤーの回転角

	//移動可能状態か
	bool m_isCanMove = true;

	//攻撃中かどうか
	bool m_isAttacking = false;

	//エフェクトを出したか
	bool m_isCreateEffect = false;

	//現在のY軸回転角
	float m_currentAngleY = 0.0f;

	//着地音を出したか
	bool m_isPlayLandSE = false;

	//ステート
	enum class State : int
	{
		Idle,//待機中
		Attack,//攻撃
		Move,//移動
		Damage,//ダメージを受けたとき
	};

	//現在のステート
	State m_currentState = State::Idle;

	//アニメーター
	ModelAnimator m_animator;

	//攻撃判定用の球
	Sphere m_attackSphere;
	//攻撃判定用の球の半径
	//攻撃時のみ半径を大きくして、攻撃範囲を広げる
	float m_attackSphereR = 0.0f;

	//エフェクトマネージャーへのポインタ
	std::shared_ptr<EffectManager> m_pEffectManager;

	//カメラへのポインタ
	std::shared_ptr<Camera> m_pCamera;

private:
	/// <summary>
	/// 移動
	/// </summary>
	/// <param name="input">入力情報</param>
	void Move(Input input, float angle);

	/// <summary>
	/// ジャンプ攻撃
	/// </summary>
	/// <param name="input">入力情報</param>
	void Attack();

	/// <summary>
	/// 移動中かを返す
	/// </summary>
	/// <returns>移動中か</returns>
	bool const IsMoving() const;

	/// <summary>
	/// ステートを切り替えるときに使う
	/// </summary>
	/// <param name="next">切り替えたいステート</param>
	/// <param name="force">強制的に切り替えるかどうか</param>
	void ChangeState(State next, bool force = false);

	/// <summary>
	/// 当たり判定用の球の位置と半径を更新する
	/// </summary>
	void UpdateCollSphere();

	/// <summary>
	///　プレイヤーの前方向を取得する
	/// </summary>
	/// <returns>プレイヤーの前方向ベクトル</returns>
	Vector3 GetForward() const;
};