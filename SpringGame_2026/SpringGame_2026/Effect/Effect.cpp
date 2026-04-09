#include "Effect.h"
#include <EffekseerForDXLib.h>

Effect::Effect(const Vector3& pos):
	GameObject(pos)
{

}

Effect::~Effect()
{

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