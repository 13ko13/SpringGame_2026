#pragma once
#include "GameObject.h"
#include "../System/Animator.h"

class Enemy : public GameObject
{
public:
	Enemy(int modelHandle);
	~Enemy();

	void Update() override;
	void Draw() override;

private:
	//ステート
	enum class State : int
	{
		Idle,//待機中
		Attack,//攻撃
		Move,//移動
	};

	//現在のステート
	State m_currentState = State::Idle;

	//アニメーター
	Animator m_animator;

	/// <summary>
	/// ステートを切り替えるときに使う
	/// </summary>
	/// <param name="next">切り替えたいステート</param>
	void ChangeState(State next);
};

