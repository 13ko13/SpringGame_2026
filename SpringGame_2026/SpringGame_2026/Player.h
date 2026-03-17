#pragma once
#include "GameObject.h"
class Player : public GameObject
{
public:
	Player();
	~Player();
	
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;
};

