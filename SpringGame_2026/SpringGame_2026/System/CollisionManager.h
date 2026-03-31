#pragma once
#include <memory>
#include <list>

//前方宣言
class Player;
class EnemyFactory;
class Sphere;
class Enemy;

class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();

	void Update(
		std::shared_ptr<Player>& pPlayer,
		std::shared_ptr<EnemyFactory>& pEnemyFactory
	);

private:
	/// <summary>
	/// プレイヤーの攻撃判定球と敵の当たり判定球の衝突判定
	/// </summary>
	/// <param name="attackSphere">プレイヤーの攻撃判定用の球</param>
	/// <param name="pEnemy">敵の当たり判定用の球</param>
	/// <returns>true:あたっている,false:当たっていない</returns>
	bool CheckCollision(const Sphere& attackSphere,
		std::shared_ptr<Enemy>& pEnemy);
};

