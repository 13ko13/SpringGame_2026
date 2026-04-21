#include "CollisionManager.h"
#include "../GameObjects/Player.h"
#include "../Math/Sphere.h"
#include "../GameObjects/Enemy.h"
#include "EnemyFactory.h"
#include "EffectManager.h"
#include "SoundManager.h"

namespace
{
	//何体ヒットまでなら処理をおこなうか
	constexpr int max_hit_enemy_num = 3;
}

CollisionManager::CollisionManager(std::shared_ptr<EffectManager> pEffectManager) :
	m_pEffectManager(pEffectManager)
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
		//攻撃が当たった敵の数をリセットする
		m_hitEnemyNum = 0;

		//敵一体一体とプレイヤーの攻撃判定用の球との当たり判定を行う
		for (auto& pEnemy : enemies)
		{
			//既に死亡状態の敵はスキップする
			if (!pEnemy->IsDeadAnimEnd()) continue;

			if (CheckCollision(pPlayer->GetAttackSphere(), pEnemy))
			{
				//敵の位置を取得する
				Vector3 enemyPos = pEnemy->GetPos();

				//攻撃に当たった敵の数を数える
				m_hitEnemyNum++;
				
				//攻撃が3体目の敵までは当たった処理を行う
				if (m_hitEnemyNum <= max_hit_enemy_num)
				{
					//プレイヤーの敵に攻撃が当たったときの処理を行わせる
					pPlayer->OnHitEnemy(enemyPos);
				}
				
				//敵に攻撃が当たっているときの処理を行わせる
				pEnemy->OnDead();
			}
		}
	}

	//敵の当たり判定を行う
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

		//敵一体一体の押し戻しの処理
		for (auto& other : enemies)
		{
			pushBackEnemy(pEnemy, other);
		}
	}

	//プレイヤーと敵の押し戻しの処理
	for (auto& pEnemy : enemies)
	{
		//敵が死亡済み、または死亡アニメーション中なら押し戻しの処理を行わない
		if (pEnemy->IsDead() || !pEnemy->IsDeadAnimEnd()) continue;
		pushBackPlayer(pPlayer, pEnemy);
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

void CollisionManager::pushBackEnemy(std::shared_ptr<Enemy>& pEnemy, std::shared_ptr<Enemy>& other)
{
	//自分自身との押し戻しは行わない
	if (pEnemy == other) return;
	//2体の距離ベクトルを求める
	Vector3 toOther = other->GetPos() - pEnemy->GetPos();
	//そのベクトルの長さを求める
	float distance = toOther.Length();

	//自分の球の半径
	const float myRadius = pEnemy->GetSphere().GetRadius();
	//他の敵の球の半径
	const float otherRadius = other->GetSphere().GetRadius();

	//距離がそれぞれの円の半径の和より小さいとき、押し戻しの処理を行う
	if (distance < myRadius + otherRadius)
	{
		//押し戻す方向と強さを表すベクトルを求める
		Vector3 pushVector = toOther.Normalized() * (myRadius + otherRadius - distance);
		//押し戻す処理を行う
		pEnemy->OnPushBack(-pushVector);
		other->OnPushBack(pushVector);
	}
}

void CollisionManager::pushBackPlayer(std::shared_ptr<Player>& pPlayer, std::shared_ptr<Enemy>& pEnemy)
{
	//2体の距離ベクトルを求める
	Vector3 toEnemy = pEnemy->GetPos() - pPlayer->GetPos();
	//そのベクトルの長さを求める
	float distance = toEnemy.Length();

	//プレイヤーの球の半径
	const float playerRadius = pPlayer->GetSphere().GetRadius();
	//敵の球の半径
	const float enemyRadius = pEnemy->GetSphere().GetRadius();

	//距離がそれぞれの円の半径の和より小さいとき、押し戻しの処理を行う
	if (distance < playerRadius + enemyRadius)
	{
		//押し戻す方向と強さを表すベクトルを求める
		Vector3 pushVector = toEnemy.Normalized() * (playerRadius + enemyRadius - distance);
		//押し戻す処理を行う
		pPlayer->OnPushBack(-pushVector);
	}
}