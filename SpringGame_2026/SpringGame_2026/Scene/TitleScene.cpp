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
#include "../Effect/DeathEffect.h"

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

	//後ろのほうで出す雷エフェクトの感覚
	constexpr int thunder_effect_interval = 45;

	//カメラから雷エフェクトが出るときの距離
	constexpr float thunder_effect_distance = 1500.0f;

	//雷のy座標のオフセット
	constexpr float thunder_effect_y_offset = 500.0f;
	//雷のx座標のオフセットの範囲
	constexpr float effect_x_offset_range = 1200.0f;
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

	//プレイヤーのモデルのハンドルをResourceLoaderから取得する
	handle = MV1DuplicateModel(loader.GetModel(ResourceLoader::ModelID::Player));
	m_pPlayer = std::make_shared<Player>(handle, m_pEffectManager);
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

	//TODO:タイトルロゴの画像を読み込んだり、プレイヤーモデルを読み込んだりする
	//タイトルロゴのハンドルを受け取る
	m_graphHandles.push_back(loader.GetGraphic(ResourceLoader::GraphicID::TitleLogoEffect));
	m_graphHandles.push_back(loader.GetGraphic(ResourceLoader::GraphicID::TitleLogo));
	
	//タイトルのBGMを再生する
	SoundManager::GetInstance().PlayFadeIn(SoundManager::SoundType::TitleBgm, fade_frame, true);

	//フォントのハンドルを作成する
	m_startFontHandle = CreateFontToHandle(font_name, title_font_size, -1, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
}

void TitleScene::Update(Input& input)
{
	//エフェクトのタイマー更新
	m_effectTimer++;

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
	//ウィンドウの中心にタイトルロゴを描画する
	const auto& windowSize = Application::GetInstance().GetWindowSize();
	Vector3 logoPos = { windowSize.w / 2.0f, windowSize.h / 2.0f, 0.0f };
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

	//画面下部に「Press A Button」の文字を描画する
	std::string startText = "Press A Button";
	//描画する文字列の横幅を取得する
	int textWidth = GetDrawStringWidthToHandle(startText.c_str(), static_cast<int>(startText.length()), m_startFontHandle);
	//画面下部の中心に描画するため、描画位置を計算する
	Vector3 textPos = { windowSize.w / 2.0f - textWidth / 2.0f, windowSize.h * title_logo_pos_y_rate, 0.0f };
	//描画する
	DrawStringToHandle(static_cast<int>(textPos.m_x), static_cast<int>(textPos.m_y),
		startText.c_str(), 0xffffff, m_startFontHandle);

	//TODO:タイトルロゴの画像を描画したり、モデルを描画したりする
}
