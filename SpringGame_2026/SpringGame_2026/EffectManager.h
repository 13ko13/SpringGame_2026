#pragma once
#include "Math/Vector3.h"
#include <list>

class Effect;

class EffectManager
{
public:
	EffectManager();
	~EffectManager();

	void Update();
	void Draw();

	void Create(const Vector3& pos);

private:
	//エフェクトの種類
	enum class EffectType
	{
		Explosion,	//爆発エフェクト

		EffectTypeNum,//エフェクトの種類の数
	};

	//登録されたエフェクトを格納する配列
	std::list <Effect> m_effects;
};