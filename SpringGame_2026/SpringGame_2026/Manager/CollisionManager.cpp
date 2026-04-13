#include "CollisionManager.h"
#include "../GameObjects/Player.h"
#include "../Math/Sphere.h"
#include "../GameObjects/Enemy.h"
#include "EnemyFactory.h"

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

	//プレイヤーのジャンプ攻撃と敵の当たり判定を行う
	//攻撃中のみ当たり判定を行う
	if (pPlayer->IsAttacking())
	{
		//敵一体一体とプレイヤーの攻撃判定用の球との当たり判定を行う
		for (auto& pEnemy : enemies)
		{
			//既に死亡状態の敵はスキップする
			if (!pEnemy->IsDeadAnimEnd()) continue;

			if (CheckCollision(pPlayer->GetAttackSphere(), pEnemy))
			{
				//敵に攻撃が当たっているときの処理を行わせる
				pEnemy->OnDead();
			}
		}
	}

	//敵とプレイヤーの当たり判定を行う
	//敵一体一体とプレイヤーの攻撃判定用の球との当たり判定を行う
	for (auto& pEnemy : enemies)
	{
		//敵が死亡済み、または死亡アニメーション中なら
		//当たり判定を行わない
		if (pEnemy->IsDead() || !pEnemy->IsDeadAnimEnd()) continue;

		//敵とプレイヤーの当たり判定を行う
		if (CheckCollision(pPlayer, pEnemy))
		{
			//敵とプレイヤーが当たっているときの処理を行わせる
			OnHitEnemyAndPlayer(pPlayer, pEnemy);
		}
	}
}

bool CollisionManager::CheckCollision(const Sphere& attackSphere,
		std::shared_ptr<Enemy>& pEnemy)
{
	return attackSphere.HitCollision(pEnemy->GetSphere());
}

bool CollisionManager::CheckCollision(std::shared_ptr<Player>& pPlayer,
	std::shared_ptr<Enemy>& pEnemy)
{
	return pPlayer->GetSphere().HitCollision(pEnemy->GetSphere());
}

void CollisionManager::OnHitEnemyAndPlayer(std::shared_ptr<Player>& pPlayer,
	std::shared_ptr<Enemy>& pEnemy)
{
	//プレイヤーと敵が当たったときの処理を行わせる
	pPlayer->OnDamage();
}

void CollisionManager::OnHitEnemyAndPlayerAttack(std::shared_ptr<Player>& pPlayer, std::shared_ptr<Enemy>& pEnemy)
{
	pEnemy->OnDead();
}
