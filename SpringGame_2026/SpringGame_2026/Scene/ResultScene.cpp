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
#include "../ToKanji.h"

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

	//フォント名
	const char* font_name = "玉ねぎ楷書激無料版v7改";

	//フォントのサイズ
	constexpr int score_font_size = 64;
}

ResultScene::ResultScene(SceneController& controller,const int score) :
	Scene(controller),
	m_score(score)
{
}

ResultScene::~ResultScene()
{

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

	//フォントのハンドルを取得する
	m_scoreFontHandle = CreateFontToHandle(font_name, score_font_size, -1, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
}

void ResultScene::Update(Input& input)
{
	//プレイヤーの更新
	m_pPlayer->Update(input, m_pCamera->GetAngleY(), stage_size, false);

	//何かしらのボタンが押されたらゲームシーンに遷移する
	if (input.IsTriggered("ok"))
	{
		m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller), fade_frame);
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

	//スコアの描画
	std::string scoreText = "Score:" + ToKanji::NumToKanji(m_score) + "点";

	//TODO:タイトルロゴの画像を描画したり、モデルを描画したりする
}


