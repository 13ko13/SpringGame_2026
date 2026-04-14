#pragma once
#include "Effect.h"

class AttackFieldEffect : public Effect
{
public:
	AttackFieldEffect(int handle, const Vector3& pos);
	~AttackFieldEffect();

	void Update() override;
	void Draw() override;

private:
	
};

