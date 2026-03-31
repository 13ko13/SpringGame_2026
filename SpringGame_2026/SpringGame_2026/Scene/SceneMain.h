#pragma once
#include <vector>
#include <memory>

//前方宣言
class Player;
class Input;
class Camera;
class Enemy;
class CollisionManager;
class EnemyFactory;

class SceneMain
{
public:
	SceneMain();
	~SceneMain();

	void Init();
	void Update(Input& input);
	void Draw();
private:
	void DrawGrid();

private:
	int m_frameCount;

	enum class EnemyModelType : int
	{
		Enemy1 = 0,
	};

	//プレイヤーモデルのハンドル
	int m_playerMHandle;
	int m_playerCopyMHandle;

	//敵のモデルのハンドル
	std::vector<int> m_enemyBaseMHandles;
	std::vector<int> m_enemyCopyHandles;

	//プレイヤーの実体
	std::shared_ptr<Player> m_pPlayer;

	//カメラの実体
	std::shared_ptr<Camera> m_pCamera;

	//当たり判定の管理クラスの実体
	std::shared_ptr<CollisionManager> m_pCollManager;

	//敵の生成を行う工場の実体
	std::shared_ptr<EnemyFactory> m_pEnemyFactory;
};