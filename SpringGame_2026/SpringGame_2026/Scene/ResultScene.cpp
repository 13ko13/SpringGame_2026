#include <DxLib.h>
#include <EffekseerForDXLib.h>

#include "ResultScene.h"
#include "../Main/Application.h"
#include "../GameObjects/Player.h"
#include "../Manager/EffectManager.h"
#include "../System/Input.h"
#include "../Loader/ResourceLoader.h"
#include "SceneController.h"
#include "../System/Camera.h"
#include "../Graphic/SkyBox.h"
#include "TitleScene.h"
#include "../System/ToKanji.h"
#include "../Manager/SoundManager.h"
#include "../UI/TextUI.h"

namespace
{
	//地面の場所
	const Vector3 ground_pos = { 0.0f, -50.0f, 0.0f };

	//ゲームシーンに遷移するときのフェードにかけるフレーム数
	constexpr float fade_frame = 60.0f;

	//カメラのターゲットの位置
	const Vector3 camera_target_pos = { 0.0f, 400.0f, 0.0f };

	//ステージのサイズ
	const Vector3 stage_size = { 1100.0f, 0.0f, 1100.0f };

	//フォント名
	const char* font_name = "玉ねぎ楷書激無料版v7改";

	//フォントのサイズ
	constexpr int score_font_size = 180;
	//スコアの文字のエッジの太さ
	constexpr int score_font_edge_size = 5;

	//スコアの文字が小さくなる演出にかけるフレーム数
	constexpr int score_shrink_frame = 50;

	//文字を小さくするときの最初の大きさ
	constexpr float clear_start_scale = 4.0f;

	//スコアが100点のときの特別な色
	constexpr int score_100_color = 0xff0000;
	//最大スコア
	constexpr int max_score = 100;
	//スコアを表示するときの画面割合
	constexpr float score_pos_x_rate = 0.5f;
	constexpr float score_pos_y_rate = 0.4f;

	//タイトルに戻るフォントのサイズ
	constexpr int title_font_size = 60;

	//タイトルに戻るフォントのエッジの太さ
	constexpr int title_font_edge_size = 3;

	//タイトルに戻る文字列の位置
	const float back_to_title_pos_x_rate = 0.5f;
	const float back_to_title_pos_y_rate = 0.7f;

	//タイトルに戻る文字列の点滅の最小透明度
	constexpr int back_min_alpha = 50;
	//タイトルに戻る文字列の点滅の最大透明度
	constexpr int back_max_alpha = 185;
	//タイトルに戻る文字列の点滅の周期
	constexpr float back_blink_interval = 0.05f;
}

ResultScene::ResultScene(SceneController& controller, const int score) :
	Scene(controller),
	m_score(score)
{
}

ResultScene::~ResultScene()
{
	//BGＭをフェードアウトする
	SoundManager::GetInstance().FadeOut(SoundManager::SoundType::ResultBgm, fade_frame);
}

void ResultScene::Init()
{
	// 地面のモデルのハンドルを ResourceLoader から取得する
	auto& loader = ResourceLoader::GetInstance();
	int handle = loader.GetModel(ResourceLoader::ModelID::Ground);
	// モデルのハンドルを配列に入れる
	m_modelHandles.push_back(handle);
	MV1SetPosition(m_modelHandles[static_cast<int>(ModelType::Ground)], ground_pos.ToDxLib());

	m_pEffectManager = std::make_shared<EffectManager>(
		loader.GetEffect(ResourceLoader::EffectID::Death), loader.GetEffect(ResourceLoader::EffectID::AttackField));

	//プレイヤーのモデルのハンドルをResourceLoaderから取得する
	handle = MV1DuplicateModel(loader.GetModel(ResourceLoader::ModelID::Player));
	m_pPlayer = std::make_shared<Player>(handle, m_pEffectManager, m_pCamera);
	//カメラの実体を確保
	//まだプレイヤーがいないので、仮で注視点の位置を(0,400,0)にしておく
	m_pCamera = std::make_shared<Camera>(camera_target_pos);

	//skyboxの実体を確保
	m_pSkyBox = std::make_shared<SkyBox>(
		loader.GetGraphic(ResourceLoader::GraphicID::FrontSky),
		loader.GetGraphic(ResourceLoader::GraphicID::BackSky),
		loader.GetGraphic(ResourceLoader::GraphicID::LeftSky),
		loader.GetGraphic(ResourceLoader::GraphicID::RightSky),
		loader.GetGraphic(ResourceLoader::GraphicID::UpSky),
		loader.GetGraphic(ResourceLoader::GraphicID::DownSky));

	//フォントのハンドルを取得する
	m_scoreFontHandle = CreateFontToHandle(font_name, score_font_size, -1, DX_FONTTYPE_ANTIALIASING_EDGE_4X4, -1, score_font_edge_size);
	//フォントのハンドルを作成する
	m_backToTitleFontHandle = CreateFontToHandle(font_name, title_font_size, -1, DX_FONTTYPE_ANTIALIASING_EDGE_4X4, -1, title_font_edge_size);

	//スコアテキストUIの実体を確保
	m_pScoreTextUI = std::make_shared<TextUI>(m_scoreFontHandle, score_font_size);
	//タイトルに戻るテキストUIの実体を確保
	m_pBackToTitleTextUI = std::make_shared<TextUI>(m_backToTitleFontHandle, title_font_size);

	//リザルトシーンのBGMを再生する
	SoundManager::GetInstance().PlayFadeIn(SoundManager::SoundType::ResultBgm, fade_frame, true);
}

void ResultScene::Update(Input& input)
{
	//エフェクトの更新
	UpdateEffekseer3D();

	//プレイヤーの更新
	m_pPlayer->Update(input, m_pCamera->GetAngleY(), stage_size, false);

	//フェードが終わっていなければ処理を飛ばす
	if (m_controller.GetFade().IsFading()) return;

	//フレームの更新
	m_frame++;

	//文字点滅用のタイマーの更新
	m_blinkFrame++;

	//演出が終了していて、何かしらのボタンが押されたらゲームシーンに遷移する
	if (!m_isStageing && input.IsTriggered("ok") && !m_isReturnTitle)
	{
		m_isReturnTitle = true;
		m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller), fade_frame);
		//決定音を鳴らす
		SoundManager::GetInstance().Play(SoundManager::SoundType::Decision);
	}

	//演出中にokボタンが押されたら、演出をスキップする
	if (m_isStageing && input.IsTriggered("ok"))
	{
		m_isSkip = true;
		m_isStageing = false;
	}

	
}

void ResultScene::Draw()
{
	//skyboxの描画
	m_pSkyBox->Draw(m_pCamera->GetPos());

	//地面のモデルを描画する
	MV1DrawModel(m_modelHandles[static_cast<int>(ModelType::Ground)]);

	//カメラの描画
	m_pCamera->Draw();

	//プレイヤーの描画
	m_pPlayer->Draw();

	//フェードが終わっていればテキストUIの描画をおこなう
	if (m_controller.GetFade().IsFading()) return;
	//テキストUIの描画
	DrawTextUI();
}

void ResultScene::DrawTextUI()
{
	//スコアの描画
	std::string scoreText = "成果:" + ToKanji::NumToKanji(m_score) + "点";

	//文字のサイズをt秒かけてn倍の大きさから当倍に(大→小)にしてスタンプのように描画する
	float scale = (1.0f - static_cast<float>(m_frame) / score_shrink_frame) * clear_start_scale;
	if (scale < 1.0f)
	{
		scale = 1.0f;
		//演出終了
		m_isStageing = false;

		//点数が出るときの音を鳴らす
		if (!m_isSkip)
		{
			if (!m_isPlayScoreSE)
			{
				SoundManager::GetInstance().Play(SoundManager::SoundType::ScorePop);
				m_isPlayScoreSE = true;
			}
		}
	}
	else
	{
		//演出中
		m_isStageing = true;
		//もし演出スキップしていたら、演出終了
		if (m_isSkip)
		{
			//スコアの文字の大きさを当倍にする
			scale = 1.0f;
			m_isStageing = false;
			//点数が出るときの音を鳴らす
			if (!m_isPlayScoreSE)
			{
				SoundManager::GetInstance().Play(SoundManager::SoundType::ScorePop);
				m_isPlayScoreSE = true;
			}
		}
	}

	//スコアが百点の場合のみ、特別な色で描画する
	if (m_score == max_score)
	{
		m_pScoreTextUI->DrawAtRate(scoreText, score_pos_x_rate, score_pos_y_rate, score_100_color, scale, scale, 0x000000);
	}
	else
	{
		m_pScoreTextUI->DrawAtRate(scoreText, score_pos_x_rate, score_pos_y_rate, 0x000000, scale, scale, 0xffffff);
	}

	//タイトルに戻るの文字の描画
	std::string backToTitleText = "Aボタンでタイトルに戻る";

	//点滅させるために、透明度をsin波で変化させる
	//透明度を0～255の範囲で変化させる
	//最小透明度 + 変化幅 * sin波の絶対値
	float alpha = back_min_alpha + back_max_alpha * std::abs(std::sin(m_blinkFrame * back_blink_interval));

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha));
	//描画する
	m_pBackToTitleTextUI->DrawAtRate(backToTitleText, back_to_title_pos_x_rate, back_to_title_pos_y_rate, 0x000000, 1.0, 1.0, 0xffffff);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}


