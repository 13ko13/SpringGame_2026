#include "AttackFieldEffect.h"
#include "../../../Dxlib_h/EffekseerForDXLib.h"

namespace
{
	//エフェクトの再生速度
	constexpr float effect_speed = 1.5f;
}

AttackFieldEffect::AttackFieldEffect(int handle, const Vector3& pos):
	Effect(pos)
{
	m_resourceHandle = handle;
	if (m_resourceHandle >= 0)
	{
		//m_playingHandleにエフェクトの再生を開始する
		m_playingHandle = PlayEffekseer3DEffect(m_resourceHandle);
		SetSpeedPlayingEffekseer3DEffect(m_playingHandle, effect_speed);
		auto manager = GetEffekseer3DManager();
		//再生するトリガーにした番号を第二引数に指定する
		manager.Get()->SendTrigger(m_playingHandle, 0);
		//エフェクトの位置を設定
		SetPosPlayingEffekseer3DEffect(m_playingHandle, m_pos.m_x, m_pos.m_y, m_pos.m_z);
		//m_handleの値が無効であればアサートする
		assert(m_playingHandle >= 0);
	}
}

AttackFieldEffect::~AttackFieldEffect()
{
	//エフェクトの再生を停止する
	StopEffekseer3DEffect(m_playingHandle);

	m_playingHandle = -1;
}

void AttackFieldEffect::Update()
{
	//もしm_playingHandleに有効な値が入っている場合は、エフェクトの位置を更新する
	if (m_playingHandle >= 0)
	{
		//エフェクトの位置を更新する
		SetPosPlayingEffekseer3DEffect(m_playingHandle, m_pos.m_x, m_pos.m_y, m_pos.m_z);
	}
}

void AttackFieldEffect::Draw()
{
}
