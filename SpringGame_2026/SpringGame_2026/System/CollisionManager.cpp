#include "CollisionManager.h"
#include "../GameObjects/Player.h"
#include "../Math/Sphere.h"
#include "../GameObjects/Enemy.h"
#include "../System/EnemyFactory.h"

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::Update(std::shared_ptr<Player>& pPlayer,
	std::shared_ptr<EnemyFactory>& pEnemyFactory)
{
	//敵のリストを取得する
	auto& enemies = pEnemyFactory->GetEnemies();
	//攻撃中のみ当たり判定を行う
	if (pPlayer->IsAttacking())
	{
		//敵一体一体とプレイヤーの攻撃判定用の球との当たり判定を行う
		for (auto& pEnemy : enemies)
		{
			if (CheckCollision(pPlayer->GetAttackSphere(), pEnemy))
			{
				//敵に当たっているときの処理を行わせる
				pEnemy->OnDead();
				pPlayer->OnAttackedEnemy();
			}
		}
	}

	//敵が死んでいるときはリストから削除する
	auto it = std::remove_if(enemies.begin(), enemies.end(),
		[](const std::shared_ptr<Enemy>& enemy)
		{
			return enemy->IsDead(); 
		});

	enemies.erase(it, enemies.end());
}

bool CollisionManager::CheckCollision(const Sphere& attackSphere,
		std::shared_ptr<Enemy>& pEnemy)
{
	return attackSphere.HitCollision(pEnemy->GetSphere());
}
