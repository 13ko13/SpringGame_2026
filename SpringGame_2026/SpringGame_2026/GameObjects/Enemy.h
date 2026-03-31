#pragma once
#include "GameObject.h"
#include "../System/Animator.h"

class Enemy : public GameObject
{
public:
	Enemy(const int modelHandle, const Vector3& pos);
	~Enemy();

	void Update() override;
	void Draw() override;
	
	/// <summary>
	/// 攻撃が当てられた時の処理
	/// </summary>
	void OnHit();

	/// <summary>
	/// オブジェクトが死亡したときの処理
	/// </summary>
	void OnDead();

private:
	//ステート
	enum class State : int
	{
		Idle,//待機中
		Hit,//攻撃を受けたとき
		Dead,//死んでいるとき	
	};

	//現在のステート
	State m_currentState = State::Idle;

	//アニメーター
	Animator m_animator;

	//Hitアニメーションをおこなったか
	bool m_isHitAnim = false;

private:
	/// <summary>
	/// ステートを切り替えるときに使う
	/// </summary>
	/// <param name="next">切り替えたいステート</param>
	void ChangeState(State next);
};

