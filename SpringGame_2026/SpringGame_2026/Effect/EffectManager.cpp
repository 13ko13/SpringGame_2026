#include "EffectManager.h"
#include "Effect.h"
#include "DeathEffect.h"

EffectManager::EffectManager(const int deathEffectHandle)
{
	//エフェクトのハンドルを配列に格納する
	m_handles.push_back(deathEffectHandle);
}

EffectManager::~EffectManager()
{
	//処理なし
}

void EffectManager::Update()
{
	//エフェクトの更新
	for (auto& effect : m_pEffects)
	{
		effect->Update();
	}
}

void EffectManager::Draw()
{
	//エフェクトの描画
	for (auto& effect : m_pEffects)
	{
		effect->Draw();
	}
}

void EffectManager::Create(const Vector3& pos, EffectType type)
{
	//エフェクトの種類に応じてエフェクトを作成する
	switch (type)
	{
	case EffectType::EnemyDeath:
		m_pEffects.push_back(std::make_shared<DeathEffect>(m_handles[static_cast<int>(EffectType::EnemyDeath)], pos));
		break;
	default:
		break;
	}
}