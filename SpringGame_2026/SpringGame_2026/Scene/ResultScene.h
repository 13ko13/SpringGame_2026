#pragma once
#include "Scene.h"
#include <memory>

class Player;
class Camera;
class SkyBox;
class EffectManager;
class ResultScene : public Scene
{
public:
	ResultScene(SceneController& controller);
	~ResultScene();

	void Init() override;
	void Update(Input& input) override;
	void Draw() override;

private:
	enum class ModelType : int
	{
		Ground = 0,//地面
	};

	enum class GraphType : int
	{
		TitleLogo = 0,//タイトルロゴ
	};

	//グラフィックハンドルの配列
	std::vector<int> m_graphHandles;
	//モデルハンドルの配列
	std::vector<int> m_modelHandles;

	//カメラの実体
	std::shared_ptr<Camera> m_pCamera;

	//プレイヤーの実体
	std::shared_ptr<Player> m_pPlayer;

	//skyboxの実体
	std::shared_ptr<SkyBox> m_pSkyBox;

	//エフェクトマネージャーの実体
	std::shared_ptr<EffectManager> m_pEffectManager;
};

