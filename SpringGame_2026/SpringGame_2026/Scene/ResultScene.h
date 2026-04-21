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
	ResultScene(SceneController& controller,const int score);
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

	//スコア表示時のフォントハンドル
	int m_scoreFontHandle = -1;

	//スコア
	int m_score = 0;

	//文字を小さくするとき用のフレームカウンター
	int m_frame = 0;

	//演出スキップするか
	bool m_isSkip = false;

	//演出中か
	bool m_isStageing = false;

	//タイトルに戻るボタンを押したか
	bool m_isReturnTitle = false;

	//スコアを表示する音を鳴らしたか
	bool m_isPlayScoreSE = false;

	//カメラの実体
	std::shared_ptr<Camera> m_pCamera;

	//プレイヤーの実体
	std::shared_ptr<Player> m_pPlayer;

	//skyboxの実体
	std::shared_ptr<SkyBox> m_pSkyBox;

	//エフェクトマネージャーの実体
	std::shared_ptr<EffectManager> m_pEffectManager;
};

