#include "EnemyPortalEffect.h"
#include "../../../Dxlib_h/EffekseerForDXLib.h"

EnemyPortalEffect::EnemyPortalEffect(int handle, const Vector3& pos):
	Effect(pos)
{
	m_resourceHandle = handle;
	if (m_resourceHandle >= 0)
	{
		//m_playingHandleにエフェクトの再生を開始する
		m_playingHandle = PlayEffekseer3DEffect(m_resourceHandle);
		//エフェクトの位置を設定
		SetPosPlayingEffekseer3DEffect(m_playingHandle, m_pos.m_x, m_pos.m_y, m_pos.m_z);
		//m_handleの値が無効であればアサートする
		assert(m_playingHandle >= 0);
	}
}

EnemyPortalEffect::~EnemyPortalEffect()
{
	//エフェクトの再生を停止する
	StopEffekseer3DEffect(m_resourceHandle);

	m_playingHandle = -1;
}

void EnemyPortalEffect::Update()
{
	//もしm_playingHandleに有効な値が入っている場合は、エフェクトの位置を更新する
	if (m_playingHandle >= 0)
	{
		//エフェクトの位置を更新する
		SetPosPlayingEffekseer3DEffect(m_playingHandle, m_pos.m_x, m_pos.m_y, m_pos.m_z);
	}
}

void EnemyPortalEffect::Draw()
{
}
