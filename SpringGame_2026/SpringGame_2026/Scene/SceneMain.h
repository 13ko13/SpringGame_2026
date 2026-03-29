#pragma once
#include <vector>
#include <memory>


//前方宣言
class Player;
class Input;
class Camera;
class Enemy;

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

	enum class ModelType : int
	{
		Player = 0,
		Enemy = 1,
	};

	//プレイヤーや敵のモデルのハンドル
	std::vector<int> m_modelBaseHandles;
	std::vector<int> m_modelCopyHandles;

	//プレイヤーの実体
	std::shared_ptr<Player> m_pPlayer;

	//カメラの実体
	std::shared_ptr<Camera> m_pCamera;

	//敵の実体
	std::shared_ptr<Enemy> m_pEnemy;
};