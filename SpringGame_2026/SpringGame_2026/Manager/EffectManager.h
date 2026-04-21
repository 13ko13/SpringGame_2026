#pragma once
#include "../Math/Vector3.h"
#include <list>
#include <memory>
#include <vector>

class Effect;

class EffectManager
{
public:
	EffectManager(const int deathEffectHandle,const int attackFieldEffectHandle);
	~EffectManager();

	void Update();
	void Draw();

	//エフェクトの種類
	enum class EffectType
	{
		HitEnemy,//敵の死亡エフェクト
		AttackField,//攻撃エフェクト

		EffectTypeNum,//エフェクトの種類の数
	};

	/// <summary>
	/// エフェクトを作成する
	/// </summary>
	/// <param name="pos">エフェクトの位置</param>
	/// <param name="type">エフェクトの種類</param>
	void Create(const Vector3& pos, EffectType type);

	/// <summary>
	/// エフェクトが再生中かどうかを返す
	/// </summary>
	/// <param name="handle">エフェクトのハンドル</param>
	/// <returns>true:再生中,false:再生中でない</returns>
	bool IsPlaying(int handle) const;

	/// <summary>
	/// 全てのエフェクトを停止する
	/// </summary>
	void StopAllEffects();

private:
	//登録されたエフェクトを格納する配列
	std::list<std::shared_ptr<Effect>> m_pEffects;
	//エフェクトのハンドル全てを格納する配列
	std::vector<int> m_handles;
	//現在再生中のAttackFieldエフェクトの再生ハンドル
	int m_attackFieldPlayingHandle = -1;
};