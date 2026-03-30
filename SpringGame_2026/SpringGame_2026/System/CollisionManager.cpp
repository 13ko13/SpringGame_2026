#include "CollisionManager.h"
#include "../GameObjects/Player.h"
#include "../Math/Sphere.h"
#include "../GameObjects/Enemy.h"

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::Update(std::shared_ptr<Player>& pPlayer, std::shared_ptr<Enemy>& pEnemy)
{
	//攻撃中のみ当たり判定を行う
	if (pPlayer->IsAttacking())
	{
		if (CheckCollision(pPlayer->GetAttackSphere(), pEnemy))
		{
			//敵に当たっているときの処理を行わせる
			pEnemy->OnHit();
			pPlayer->OnAttackedEnemy();
		}
	}
}

bool CollisionManager::CheckCollision(const Sphere& attackSphere,
		std::shared_ptr<Enemy>& pEnemy)
{
	return attackSphere.HitCollision(pEnemy->GetSphere());
}
