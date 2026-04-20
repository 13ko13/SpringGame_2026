#pragma once
#include "Scene.h"
#include "../System/SpriteAnimator.h"
#include "../System/ModelAnimator.h"
#include "../System/Camera.h"
#include "../Graphic/SkyBox.h"
#include "../GameObjects/Player.h"
#include <memory>


class TitleScene : public Scene
{
public:
	TitleScene(SceneController& controller);
	~TitleScene();

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
		TitleLogoMozi = 0,//タイトルロゴの文字画像
		TitleLogoEffect = 1,//タイトルロゴのエフェクトの画像
	};

	//グラフィックハンドルの配列
	std::vector<int> m_graphHandles;
	//モデルハンドルの配列
	std::vector<int> m_modelHandles;

	//フォントハンドル
	int m_startFontHandle = -1;

	//エフェクトを出すためのタイマー
	int m_effectTimer = 0;

	//Aボタンを押したかどうか
	bool m_isStart = false;

	//カメラの実体
	std::shared_ptr<Camera> m_pCamera;

	//プレイヤーの実体
	std::shared_ptr<Player> m_pPlayer;

	//skyboxの実体
	std::shared_ptr<SkyBox> m_pSkyBox;

	//エフェクトマネージャーの実体
	std::shared_ptr<EffectManager> m_pEffectManager;
};