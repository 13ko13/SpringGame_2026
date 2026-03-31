#pragma once
#include <memory>
#include <list>
#include <vector>

//前方宣言
class Enemy;

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
	EnemyFactory(std::vector<int> modelHandles);
	~EnemyFactory();

	/// <summary>
	/// 敵の生成を行う
	/// </summary>
	/// <param name="pos">生成させる位置</param>
	/// <returns>敵のポインタ</returns>
	std::shared_ptr<Enemy> Create(const Vector3& pos, EnemyType enemyType);

	/// <summary>
	/// 生成されている敵のリストを返す
	/// </summary>
	/// <returns>生成されている敵のリスト</returns>
	std::list<std::shared_ptr<Enemy>>& GetEnemies() { return m_enemies; }

	/// <summary>
	/// 敵全員のUpdate
	/// </summary>
	void Update();

	/// <summary>
	/// 敵全員のDraw
	/// </summary>
	void Draw();

private:
	//あらかじめ敵の表示に必要なモデルをロードしておく
	std::vector<int> m_handles;//敵全員のハンドルを持っておく

	std::list<std::shared_ptr<Enemy>> m_enemies;//生成した敵を保存しておくリスト
};

