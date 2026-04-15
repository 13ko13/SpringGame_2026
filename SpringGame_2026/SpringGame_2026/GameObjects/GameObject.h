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

	/// <summary>
	/// 当たり判定用の球を取得する
	/// </summary>
	/// <returns></returns>
	virtual Sphere const GetSphere() const { return m_sphere; }

	/// <summary>
	/// オブジェクトが死亡しているかを返す関数
	/// </summary>
	/// <returns>true:死んでいる,false:死んでいない</returns>
	virtual bool IsDead() const { return m_isDead; }

	/// <summary>
	/// 他のオブジェクトとの押し戻しの処理
	/// </summary>
	/// <param name="pushVector">押し戻す方向と強さを表すベクトル</param>
    virtual void OnPushBack(const Vector3& pushVector);

private:

protected:
	//位置と速度	
	Vector3 m_pos;
	Vector3 m_velocity;

	//死んでいるか
	bool m_isDead = false;

	//重力
	float m_gravity;
	//フレーム数
	int m_frame;

	//モデルハンドル
	int m_modelHandle;

	//当たり判定用の球
	Sphere m_sphere;
};