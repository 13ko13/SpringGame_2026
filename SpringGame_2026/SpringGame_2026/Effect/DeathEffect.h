#pragma once
#include "Effect.h"
#include "../Math/Vector3.h"

class DeathEffect : public Effect
{
public:
	DeathEffect(int handle, const Vector3& pos);
	~DeathEffect();

	void Update() override;
	void Draw() override;

private:
	
};

