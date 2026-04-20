#include "Effect.h"
#include <EffekseerForDXLib.h>

Effect::Effect(const Vector3& pos):
	GameObject(pos)
{

}

Effect::~Effect()
{
	// インスタンスが破棄される際、まだ再生中なら停止させる
	if (m_playingHandle >= 0)
	{
		StopEffekseer3DEffect(m_playingHandle);
		m_playingHandle = -1;
	}
}

void Effect::Update()
{

}

void Effect::Draw()
{

}

bool Effect::IsPlaying() const
{
	if (m_playingHandle < 0) return false;
	//IsEffekseer3DEffectPlayingは再生中なら0、終了なら-1を返す
	return IsEffekseer3DEffectPlaying(m_playingHandle) == 0;
}