#include "TitleScene.h"
#include "SceneController.h"
#include <cassert>
#include "../System/Input.h"
#include "../Loader/ResourceLoader.h"
#include "GameScene.h"
#include "../Main/Application.h"
#include "../GameObjects/Player.h"
#include "../Manager/EffectManager.h"

namespace
{
	//地面の場所
	const Vector3 ground_pos = { 0.0f, -50.0f, 0.0f };

	//ゲームシーンに遷移するときのフェードにかけるフレーム数
	constexpr float fade_frame = 60.0f;

	//タイトルロゴの位置オフセット
	const Vector3 title_logo_offset = { 0.0f, -150.0f, 0.0f };

	//タイトルロゴの大きさ
	constexpr float title_logo_scale = 0.5f;

	//カメラのターゲットの位置
	const Vector3 camera_target_pos = { 0.0f, 400.0f, 0.0f };

	//ステージのサイズ
	const Vector3 stage_size = { 1100.0f, 0.0f, 1100.0f };
}

TitleScene::TitleScene(SceneController& controller):
	Scene(controller)
{
}

TitleScene::~TitleScene()
{
	
}

void TitleScene::Init()
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
	m_graphHandles.push_back(loader.GetGraphic(ResourceLoader::GraphicID::TitleLogo));
}

void TitleScene::Update(Input& input)
{
	//プレイヤーの更新
	m_pPlayer->Update(input, m_pCamera->GetAngleY(), stage_size, false);

	//何かしらのボタンが押されたらゲームシーンに遷移する
	if (input.IsTriggered("ok"))
	{
		m_controller.ChangeScene(std::make_shared<GameScene>(m_controller), fade_frame);
	}
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

	//タイトルロゴの描画
	//ウィンドウの中心にタイトルロゴを描画する
	const auto& windowSize = Application::GetInstance().GetWindowSize();
	Vector3 logoPos = { windowSize.w / 2.0f, windowSize.h / 2.0f, 0.0f };
	logoPos += title_logo_offset;//タイトルロゴの位置オフセットを加算する
	int logoX = static_cast<int>(logoPos.m_x);
	int logoY = static_cast<int>(logoPos.m_y);
	DrawRotaGraph(logoX, logoY,
		static_cast<double>(title_logo_scale), 0.0,
		m_graphHandles[static_cast<int>(GraphType::TitleLogo)], TRUE);

	//TODO:タイトルロゴの画像を描画したり、モデルを描画したりする
}
