#pragma once
#include <vector>
#include <memory>


//前方宣言
class Player;

class SceneMain
{
public:
	SceneMain();
	~SceneMain();

	void Init();
	void Update();
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
	std::vector<int> m_modelHandles;

	//プレイヤーの実体
	std::shared_ptr<Player> m_pPlayer;
};