#include <EffekseerForDXLib.h>
#include <cassert>

#include "TitleScene.h"
#include "SceneController.h"
#include "../System/Input.h"
#include "../Loader/ResourceLoader.h"
#include "GameScene.h"
#include "../Main/Application.h"
#include "../GameObjects/Player.h"
#include "../Manager/EffectManager.h"
#include "../Manager/SoundManager.h"
#include "../UI/TextUI.h"

namespace
{
	//地面の場所
	const Vector3 ground_pos = { 0.0f, -50.0f, 0.0f };

	//ゲームシーンに遷移するときのフェードにかけるフレーム数
	constexpr float fade_frame = 60.0f;

	//タイトルロゴの位置オフセット
	const Vector3 title_logo_offset = { 0.0f, -150.0f, 0.0f };

	//タイトルロゴの大きさ
	constexpr float title_logo_scale = 0.65f;

	//カメラのターゲットの位置
	const Vector3 camera_target_pos = { 0.0f, 400.0f, 0.0f };

	//ステージのサイズ
	const Vector3 stage_size = { 1100.0f, 0.0f, 1100.0f };

	//フォント名
	const char* font_name = "玉ねぎ楷書激無料版v7改";

	//フォントのサイズ
	constexpr int title_font_size = 60;

	//タイトルロゴの描画位置割合
	constexpr float title_logo_pos_y_rate = 0.8f;

	//Aボタンでスタートの文字列の位置割合
	constexpr float start_text_pos_x_rate = 0.5f;
	constexpr float start_text_pos_y_rate = 0.8f;

	//後ろのほうで出す雷エフェクトの感覚
	constexpr int thunder_effect_interval = 45;

	//カメラから雷エフェクトが出るときの距離
	constexpr float thunder_effect_distance = 1500.0f;

	//雷のy座標のオフセット
	constexpr float thunder_effect_y_offset = 500.0f;
	//雷のx座標のオフセットの範囲
	constexpr float effect_x_offset_range = 1200.0f;

	//フォント点滅時の最小透明度
	constexpr int min_alpha = 50;
	//フォント点滅時の変化幅
	constexpr int max_alpha = 175;

	//フォント点滅時の間隔
	constexpr float blink_interval = 0.03f;


	//スタートされたときの最大透明度
	constexpr int start_max_alpha = 150;
	//スタートされたときの点滅の間隔
	constexpr float start_blink_interval = 0.5f;
}

TitleScene::TitleScene(SceneController& controller) :
	Scene(controller)
{
}

TitleScene::~TitleScene()
{
	//エフェクトを全て停止する
	m_pEffectManager->StopAllEffects();
}

void TitleScene::Init()
{
	//カリングの設定
	SetUseBackCulling(true);

	// 地面のモデルのハンドルを ResourceLoader から取得する
	auto& loader = ResourceLoader::GetInstance();
	int handle = loader.GetModel(ResourceLoader::ModelID::Ground);
	// モデルのハンドルを配列に入れる
	m_modelHandles.push_back(handle);
	MV1SetPosition(m_modelHandles[static_cast<int>(ModelType::Ground)], ground_pos.ToDxLib());

	m_pEffectManager = std::make_shared<EffectManager>(
		loader.GetEffect(ResourceLoader::EffectID::Death), loader.GetEffect(ResourceLoader::EffectID::AttackField));

	//カメラの実体を確保
	m_pCamera = std::make_shared<Camera>(camera_target_pos);
	//プレイヤーのモデルのハンドルをResourceLoaderから取得する
	handle = MV1DuplicateModel(loader.GetModel(ResourceLoader::ModelID::Player));
	m_pPlayer = std::make_shared<Player>(handle, m_pEffectManager, m_pCamera);

	//skyboxの実体を確保
	m_pSkyBox = std::make_shared<SkyBox>(
		loader.GetGraphic(ResourceLoader::GraphicID::FrontSky),
		loader.GetGraphic(ResourceLoader::GraphicID::BackSky),
		loader.GetGraphic(ResourceLoader::GraphicID::LeftSky),
		loader.GetGraphic(ResourceLoader::GraphicID::RightSky),
		loader.GetGraphic(ResourceLoader::GraphicID::UpSky),
		loader.GetGraphic(ResourceLoader::GraphicID::DownSky));

	//TODO:タイトルロゴの画像を読み込んだり、プレイヤーモデルを読み込んだりする
	//タイトルロゴのハンドルを受け取る
	m_graphHandles.push_back(loader.GetGraphic(ResourceLoader::GraphicID::TitleLogoEffect));
	m_graphHandles.push_back(loader.GetGraphic(ResourceLoader::GraphicID::TitleLogo));
	
	//タイトルのBGMを再生する
	SoundManager::GetInstance().PlayFadeIn(SoundManager::SoundType::TitleBgm, fade_frame, true);

	//フォントのハンドルを作成する
	int fontType = DX_FONTTYPE_ANTIALIASING_EDGE_4X4;//エッジ付きのフォント
	m_startFontHandle = CreateFontToHandle(font_name, title_font_size, -1, fontType);
	//UIの実体を確保
	m_pStartText = std::make_shared<TextUI>(m_startFontHandle,title_font_size);
}

void TitleScene::Update(Input& input)
{
	//エフェクトのタイマー更新
	m_effectTimer++;

	//フォントの点滅のタイマー更新
	m_fontBlinkTimer++;

	//プレイヤーの更新
	m_pPlayer->Update(input, m_pCamera->GetAngleY(), stage_size, false);

	//何かしらのボタンが押されたらゲームシーンに遷移する
	if (input.IsTriggered("ok") && !m_isStart)
	{
		m_isStart = true;

		//決定音を鳴らす
		SoundManager::GetInstance().Play(SoundManager::SoundType::Decision);

		//TitleBgmをフェードアウトさせる
		SoundManager::GetInstance().FadeOut(SoundManager::SoundType::TitleBgm, fade_frame);

		m_controller.ChangeScene(std::make_shared<GameScene>(m_controller), fade_frame);
	}

	UpdateEffekseer3D(); 
	m_pEffectManager->Update();

	//数秒ごとに後ろのほうで雷エフェクトを出す
	if (m_effectTimer % thunder_effect_interval == 0)
	{
		//カメラの少し後ろの位置にエフェクトを出す
		Vector3 effectPos = m_pCamera->GetPos() + m_pCamera->GetForward() * thunder_effect_distance;
		//少し下に下げる
		effectPos.m_y -= thunder_effect_y_offset;
		//X軸を少しランダムに動かす
		effectPos.m_x += (rand() % static_cast<int>(effect_x_offset_range) * 2 - static_cast<int>(effect_x_offset_range));

		m_pEffectManager->Create(effectPos, EffectManager::EffectType::HitEnemy);
	}

	//DxLibのカメラ設定をEffekseerに同期する
	Effekseer_Sync3DSetting();
}

void TitleScene::Draw()
{
	//skyboxの描画
	m_pSkyBox->Draw(m_pCamera->GetPos());

	//地面のモデルを描画する
	MV1DrawModel(m_modelHandles[static_cast<int>(ModelType::Ground)]);

	//カメラの描画
	m_pCamera->Draw();

	//プレイヤーの描画
	m_pPlayer->Draw();

	//エフェクトの描画
	DrawEffekseer3D();
	m_pEffectManager->Draw();

	//タイトルロゴの描画
	DrawTitleLogo();

	//画面下部に「Press A Button」の文字を描画する
	std::string startText = "Aボタンで開始";

	//点滅させるために、透明度をsin波で変化させる　
	//透明度を0～255の範囲で変化させる
	//最小透明度 + 変化幅 * sin波の絶対値
	int alpha;
		
	if (!m_isStart)
	{
		alpha = static_cast<int>(min_alpha + max_alpha * std::abs(std::sin(m_fontBlinkTimer * blink_interval)));

		//描画に透明度をてきようする
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		//描画する
		m_pStartText->DrawAtRate(startText, start_text_pos_x_rate, start_text_pos_y_rate, 0x000000);
		//透明度をリセット
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else
	{
		alpha = static_cast<int>(min_alpha + start_max_alpha * std::abs(std::sin(m_fontBlinkTimer * start_blink_interval)));
		//描画に透明度をてきようする
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		//描画する
		m_pStartText->DrawAtRate(startText, start_text_pos_x_rate, start_text_pos_y_rate, 0x000000);
		//透明度をリセット
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	//TODO:タイトルロゴの画像を描画したり、モデルを描画したりする
}

void TitleScene::DrawTitleLogo() const
{
	//タイトルロゴの描画
	//ウィンドウの中心にタイトルロゴを描画する
	const auto& windowSize = Application::GetInstance().GetWindowSize();
	Vector3 logoPos = { windowSize.m_width / 2.0f, windowSize.m_height / 2.0f, 0.0f };
	logoPos += title_logo_offset;//タイトルロゴの位置オフセットを加算する
	int logoX = static_cast<int>(logoPos.m_x);
	int logoY = static_cast<int>(logoPos.m_y);
	DrawRotaGraph(logoX, logoY,
		static_cast<double>(title_logo_scale), 0.0,
		m_graphHandles[static_cast<int>(GraphType::TitleLogoMozi)], TRUE);

	//タイトルロゴのエフェクトの描画
	DrawRotaGraph(logoX, logoY,
		static_cast<double>(title_logo_scale), 0.0,
		m_graphHandles[static_cast<int>(GraphType::TitleLogoEffect)], TRUE);
}
