#pragma once
#include <memory>
#include <list>

//前方宣言
class Player;
class EnemyFactory;
class Sphere;
class Enemy;
class EffectManager;

class CollisionManager
{
public:
	CollisionManager(std::shared_ptr<EffectManager> pEffectManager);
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

	/// <summary>
	/// プレイヤーと敵の当たり判定球の衝突判定
	/// </summary>
	/// <param name="pPlayer">プレイヤーのオブジェクト</param>
	/// <param name="pEnemy">敵のオブジェクト</param>
	/// <returns>true:当たっている,false:当たっていない</returns>
	bool CheckCollision(std::shared_ptr<Player>& pPlayer,
		std::shared_ptr<Enemy>& pEnemy);

	/// <summary>
	/// プレイヤーと敵の当たり判定
	/// </summary>
	/// <param name="pPlayer"></param>
	/// <param name="pEnemy"></param>
	void OnHitEnemyAndPlayer(std::shared_ptr<Player>& pPlayer,
		std::shared_ptr<Enemy>& pEnemy);

	/// <summary>
	/// 敵とプレイヤーの攻撃判定が当たった時の処理
	/// </summary>
	/// <param name="pPlayer">プレイヤーのオブジェクト</param>
	/// <param name="pEnemy">敵のオブジェクト</param>
	void OnHitEnemyAndPlayerAttack(std::shared_ptr<Player>& pPlayer, std::shared_ptr<Enemy>& pEnemy);

	/// <summary>
	/// 敵同士の押し戻しの処理
	/// </summary>
	/// <param name="pEnemy">敵のポインタ</param>
	/// <param name="other">他の敵のポインタ</param>
	void pushBackEnemy(std::shared_ptr<Enemy>& pEnemy, std::shared_ptr<Enemy>& other);

	/// <summary>
	///	プレイヤーと敵の押し戻しの処理
	/// </summary>
	/// <param name="pPlayer">プレイヤーのオブジェクト</param>
	/// <param name="pEnemy">敵のオブジェクト</param>
	void pushBackPlayer(std::shared_ptr<Player>& pPlayer, std::shared_ptr<Enemy>& pEnemy);

private:
	std::shared_ptr<EffectManager> m_pEffectManager;//エフェクトマネージャーへのポインタ
	int m_hitEnemyNum = 0;//攻撃が当たった敵の数
};

