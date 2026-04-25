#pragma once
#include "Scene.h"
#include <memory>

class Player;
class Camera;
class SkyBox;
class EffectManager;
class TextUI;
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

	//グラフィックハンドルの配列
	std::vector<int> m_graphHandles;
	//モデルハンドルの配列
	std::vector<int> m_modelHandles;

	//スコア表示時のフォントハンドル
	int m_scoreFontHandle = -1;
	//タイトルに戻るフォントハンドル
	int m_backToTitleFontHandle = -1;

	//スコアテキストUI
	std::shared_ptr<TextUI> m_pScoreTextUI;
	//タイトルに戻るテキストUI
	std::shared_ptr<TextUI> m_pBackToTitleTextUI;

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

	//文字を点滅させるときのフレームカウンター
	int m_blinkFrame = 0;

	//カメラの実体
	std::shared_ptr<Camera> m_pCamera;

	//プレイヤーの実体
	std::shared_ptr<Player> m_pPlayer;

	//skyboxの実体
	std::shared_ptr<SkyBox> m_pSkyBox;

	//エフェクトマネージャーの実体
	std::shared_ptr<EffectManager> m_pEffectManager;

private:
	/// <summary>
	/// テキストUIを描画
	/// </summary>
	void DrawTextUI();
};

