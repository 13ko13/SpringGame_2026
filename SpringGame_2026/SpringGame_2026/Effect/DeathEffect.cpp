#include "DeathEffect.h"
#include "../../../Dxlib_h/EffekseerForDXLib.h"

DeathEffect::DeathEffect(int handle, const Vector3& pos):
	Effect(pos)
{
	m_resourceHandle = handle;
	if (m_resourceHandle >= 0)
	{
		//m_playingHandleにエフェクトの再生を開始する
		m_playingHandle = PlayEffekseer3DEffect(m_resourceHandle);
		auto manager = GetEffekseer3DManager();
		//再生するトリガーにした番号を第二引数に指定する
		manager.Get()->SendTrigger(m_playingHandle, 0);
		//エフェクトの位置を設定
		SetPosPlayingEffekseer3DEffect(m_playingHandle, m_pos.m_x, m_pos.m_y, m_pos.m_z);
		//m_handleの値が無効であればアサートする
		assert(m_playingHandle >= 0);
	}
}

DeathEffect::~DeathEffect()
{
	//エフェクトの再生を停止する
	StopEffekseer3DEffect(m_playingHandle);

	m_playingHandle = -1;
}

void DeathEffect::Update()
{
	////エフェクトの再生を停止する
	//if (m_playingHandle >= 0)
	//{
	//	//Effekseerのマネージャーを取得して、エフェクトの再生を停止する
	//	auto manager = GetEffekseer3DManager();
	//	//0番のトリガーで停止する
	//	manager.Get()->SendTrigger(m_playingHandle, 0);
	//	m_playingHandle = -1;
	//}

	//もしm_playingHandleに有効な値が入っている場合は、エフェクトの位置を更新する
	if (m_playingHandle >= 0) 
	{
		//エフェクトの位置を更新する
		SetPosPlayingEffekseer3DEffect(m_playingHandle, m_pos.m_x, m_pos.m_y,m_pos.m_z);
	}
}

void DeathEffect::Draw()
{

}
