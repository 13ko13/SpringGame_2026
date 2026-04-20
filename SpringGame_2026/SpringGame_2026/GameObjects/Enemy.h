#pragma once
#include "GameObject.h"
#include "../System/ModelAnimator.h"
#include <memory>

class EffectManager;

class Enemy : public GameObject
{
public:
	Enemy(const int modelHandle, const Vector3& pos, std::shared_ptr<EffectManager> pManager);
	~Enemy();

	void Update() override;
	void Update(const Vector3& playerPos, const Vector3& stageSize, bool isCanMove);
	void Draw() override;

	/// <summary>
	/// 攻撃が当てられた時の処理
	/// </summary>
	void OnHit();

	/// <summary>
	/// オブジェクトが死亡したときの処理
	/// </summary>
	void OnDead();

	/// <summary>
	/// 死亡アニメーションが終了したかを判定
	/// </summary>
	/// <returns>true:終了,false:まだ終了していない</returns>
	bool IsDeadAnimEnd() const;

private:
	//ステート
	enum class State : int
	{
		Idle,//待機中
		Hit, //攻撃を受けたとき
		Dead,//死んでいるとき
		Walk,//歩いているとき
	};

	//現在のステート
	State m_currentState = State::Idle;

	//アニメーター
	ModelAnimator m_animator;

	//Hitアニメーションをおこなったか
	bool m_isHitAnim = false;

	//死亡SEを出したか
	bool m_isPlayDeadSE = false;

	//現在のY軸回転角
	float m_currentAngleY = 0.0f;

private:
	/// <summary>
	/// ステートを切り替えるときに使う
	/// </summary>
	/// <param name="next">切り替えたいステート</param>
	void ChangeState(State next);

	/// <summary>
	/// 現在動いているかを判定する
	/// </summary>
	bool const IsMoving() const;

	//エフェクトマネージャー
	std::shared_ptr<EffectManager> m_pEffectManager;
};

