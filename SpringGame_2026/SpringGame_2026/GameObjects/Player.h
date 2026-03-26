#pragma once
#include "GameObject.h"
#include "../Math/Vector3.h"
#include "../System/Animator.h"

class Input;
class Player : public GameObject
{
public:
	Player(int modelHandle);
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
	void Update(Input input, float angle);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	Vector3 const GetTargetPos() const;

private:
	Vector3 m_targetPos = { 0.0f,0.0f,0.0f };//カメラに注視させる位置

	float m_prevAngleY = 0.0f;//前のフレームのプレイヤーの回転角

	//ステート
	enum class State : int
	{
		Idle,//待機中
		Attack,//攻撃
		Move,//移動
	};

	//現在のステート
	State m_currentState;

	//アニメーター
	Animator m_animator;
};