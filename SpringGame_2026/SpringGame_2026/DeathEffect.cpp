#include "DeathEffect.h"
#include "../../Dxlib_h/EffekseerForDXLib.h"

DeathEffect::DeathEffect(int handle, const Vector3& pos):
	Effect(pos),
	m_handle(handle)
{

}

DeathEffect::~DeathEffect()
{
}

void DeathEffect::Update()
{

}

void DeathEffect::Draw()
{
}
