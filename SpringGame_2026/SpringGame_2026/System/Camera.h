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

	/// <summary>
	/// Y軸回転角を取得する
	/// </summary>
	/// <returns>Y軸回転角</returns>
	float GetAngleY() const { return m_angleY; }

	/// <summary>
	/// 自身の正面ベクトルを取得する
	/// </summary>
	/// <returns>正面ベクトル</returns>
	Vector3 const GetForward() const;
	
private:
	float m_angleX = 0.0f;//カメラの回転角横
	float m_angleY = 0.0f;//カメラの回転角縦

	//ターゲットの位置
	Vector3 m_targetPos;
};