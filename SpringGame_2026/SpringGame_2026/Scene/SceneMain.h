#pragma once
#include <vector>
#include <memory>
#include "../Math/Vector3.h"

//前方宣言
class Player;
class Input;
class Camera;
class Enemy;
class CollisionManager;
class EnemyFactory;
class BackGround;

class SceneMain
{
public:
	SceneMain();
	~SceneMain();

	void Init();
	void Update(Input& input);
	void Draw();

	/// <summary>
	/// ステージのサイズを取得する
	/// </summary>
	/// <returns>ステージの大きさ</returns>
	Vector3 const GetStageSize() const;

private:
	void DrawGrid();

private:
	int m_frameCount;

	enum class EnemyModelType : int
	{
		Enemy1 = 0,
	};

	//プレイヤーモデルのハンドル
	int m_playerMHandle = -1;
	int m_playerCopyMHandle = -1;

	//敵のモデルのハンドル
	std::vector<int> m_enemyBaseMHandles;

	//プレイヤーの実体
	std::shared_ptr<Player> m_pPlayer;

	//カメラの実体
	std::shared_ptr<Camera> m_pCamera;

	//当たり判定の管理クラスの実体
	std::shared_ptr<CollisionManager> m_pCollManager;

	//敵の生成を行う工場の実体
	std::shared_ptr<EnemyFactory> m_pEnemyFactory;

	//skyboxの実体
	std::shared_ptr<BackGround> m_pBackGround;
};