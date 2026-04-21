#include "EffectManager.h"
#include "../Effect/Effect.h"
#include "../Effect/DeathEffect.h"
#include "../Effect/AttackFieldEffect.h"
#include <EffekseerForDXLib.h>

EffectManager::EffectManager(const int deathEffectHandle, const int attackFieldEffectHandle)
{
	//エフェクトのハンドルを配列に格納する
	m_handles.push_back(deathEffectHandle);
	m_handles.push_back(attackFieldEffectHandle);
}

EffectManager::~EffectManager()
{
	//再生中の全エフェクトを停止してクリア
	for(auto& effect : m_pEffects)
	{
		if (effect && effect->GetPlayingHandle() >= 0)
		{
			StopEffekseer3DEffect(effect->GetPlayingHandle());
		}
	}
	m_pEffects.clear();
}

void EffectManager::Update()
{
	//再生が終了したエフェクトをリストから削除する
	m_pEffects.remove_if([](const std::shared_ptr<Effect>& effect)
	{
		return !effect->IsPlaying();
	});

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
	case EffectType::HitEnemy:
		m_pEffects.push_back(std::make_shared<DeathEffect>(m_handles[static_cast<int>(EffectType::HitEnemy)], pos));
		break;
	case EffectType::AttackField:
	{
		//現在再生中のAttackFieldエフェクトがあるなら、新しいエフェクトは作成しない
		if (IsEffekseer3DEffectPlaying(m_attackFieldPlayingHandle) == 0)
		{
			break;
		}
		auto attackField = std::make_shared<AttackFieldEffect>(m_handles[static_cast<int>(EffectType::AttackField)], pos);
		m_pEffects.push_back(attackField);
		m_attackFieldPlayingHandle = attackField->GetPlayingHandle();
		break;
	}
	default:
		break;
	}
}

bool EffectManager::IsPlaying(int handle) const
{
	//エフェクトが再生中かどうかを返す
	return IsEffekseer3DEffectPlaying(handle) != 0;
}

void EffectManager::StopAllEffects()
{
	//再生中の全エフェクトを停止してクリア
	for (auto& effect : m_pEffects)
	{
		if (effect && effect->GetPlayingHandle() >= 0)
		{
			StopEffekseer3DEffect(effect->GetPlayingHandle());
		}
	}
	m_pEffects.clear();
}