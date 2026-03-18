#pragma once
#include "GameObject.h"

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
	void Update(Input input);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;
};