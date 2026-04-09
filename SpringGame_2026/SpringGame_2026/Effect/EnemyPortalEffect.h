#pragma once
#include "Effect.h"
#include "../Math/Vector3.h"

class EnemyPortalEffect : public Effect
{
public:
	EnemyPortalEffect(int handle, const Vector3& pos);
	~EnemyPortalEffect();
	
	void Update() override;
	void Draw() override;

private:
};

