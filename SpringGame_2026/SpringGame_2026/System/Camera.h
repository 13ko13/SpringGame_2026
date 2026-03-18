#pragma once
#include "../GameObjects/GameObject.h"

class Input;
class Camera : public GameObject
{
public:
	Camera(const Vector3& targetPos);
	~Camera();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// ターゲットの位置を受け取って、カメラの位置を更新する処理
	/// </summary>
	/// <param name="targetPos">注視点の位置</param>
	void Update(const Vector3& targetPos,const Input input);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;
	
private:
	float m_angle;//カメラの回転角
};