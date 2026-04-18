#pragma once
#include <memory>
#include <list>
#include <vector>
#include "../GameObjects/Enemy.h"

class EffectManager;

/// <summary>
/// 敵タイプ
/// </summary>
enum class EnemyType
{
	Enemy1 = 0,//敵1
};

class EnemyFactory
{
public:
	EnemyFactory(std::vector<int> modelHandles, std::shared_ptr<EffectManager> pEffectManager);
	~EnemyFactory();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(const Vector3& stageSize, const Vector3& playerPos);

	/// <summary>
	/// 敵の生成を行う
	/// </summary>
	/// <param name="pos">生成させる位置</param>
	/// <returns>敵のポインタ</returns>
	std::shared_ptr<Enemy> Create(const Vector3& pos, EnemyType enemyType, std::vector<int> enemyHandles);

	/// <summary>
	/// 生成されている敵のリストを返す
	/// </summary>
	/// <returns>生成されている敵のリスト</returns>
	std::list<std::shared_ptr<Enemy>>& GetEnemies() { return m_pEnemies; }

	/// <summary>
	/// 敵全員の更新処理
	/// </summary>
	/// <param name="playerPos">プレイヤーの位置</param>
	void Update(const Vector3& playerPos, const Vector3& stageSize, bool isCanMove);

	/// <summary>
	/// 敵全員のDraw
	/// </summary>
	void Draw();

private:
	//あらかじめ敵の表示に必要なモデルをロードしておく
	std::vector<int> m_handles;//敵全員のハンドルを持っておく

	std::list<std::shared_ptr<Enemy>> m_pEnemies;//生成した敵を保存しておくリスト

	//エフェクトマネージャー
	std::shared_ptr<EffectManager> m_pEffectManager;
};

