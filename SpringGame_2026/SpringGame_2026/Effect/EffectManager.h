#pragma once
#include "../Math/Vector3.h"
#include <list>
#include <memory>
#include <vector>

class Effect;

class EffectManager
{
public:
	EffectManager(const int deathEffectHandle);
	~EffectManager();

	void Update();
	void Draw();

	//エフェクトの種類
	enum class EffectType
	{
		EnemyDeath,//敵の死亡エフェクト

		EffectTypeNum,//エフェクトの種類の数
	};

	void Create(const Vector3& pos, EffectType type);

private:
	//登録されたエフェクトを格納する配列
	std::list<std::shared_ptr<Effect>> m_pEffects;
	//エフェクトのハンドル全てを格納する配列
	std::vector<int> m_handles;
};