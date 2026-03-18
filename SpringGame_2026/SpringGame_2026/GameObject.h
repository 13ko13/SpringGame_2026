#pragma once
#include "Vector3.h"

class GameObject
{
public:
	GameObject(int modelHandle,const Vector3& pos);
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

	virtual void const GetPos(Vector3& pos) const { pos = m_pos; }

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
};