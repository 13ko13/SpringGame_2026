#pragma once
#include "../Math/Vector3.h"
#include "../Math/Sphere.h"

class GameObject
{
public:
	GameObject(int modelHandle,const Vector3& pos = {0.0f,0.0f,0.0f});
	GameObject(const Vector3& pos);
	virtual ~GameObject();

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() abstract;

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() abstract;

	/// <summary>
	/// プレイヤーの位置を取得する
	/// </summary>
	/// <returns>プレイヤーの位置</returns>
	virtual Vector3 const GetPos() const {  return m_pos; }

	Sphere const GetSphere() const { return m_sphere; }

private:

protected:
	//位置と速度
	Vector3 m_pos;
	Vector3 m_velocity;

	//重力
	float m_gravity;
	//フレーム数
	int m_frame;

	//モデルハンドル
	int m_modelHandle;

	//当たり判定用の球
	Sphere m_sphere;
};