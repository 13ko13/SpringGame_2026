#include <DxLib.h>
#include <cassert>
#include <EffekseerForDXLib.h>

#include "GameScene.h"
#include "../GameObjects/Player.h"
#include "../System/Input.h"
#include "../System/Camera.h"
#include "../Manager/CollisionManager.h"
#include "../Manager/EnemyFactory.h"
#include "../Graphic/SkyBox.h"
#include "../Manager/EffectManager.h"
#include "../Loader/ResourceLoader.h"
#include "SceneController.h"
#include "ResultScene.h"
#include "../Main/Application.h"

namespace
{
	//カメラのnearとfar
	constexpr float camera_near = 200.0f;
	constexpr float camera_far = 1500.0f;

	//ステージのサイズ
	const Vector3 stage_size = { 1400.0f, 0.0f, 1400.0f };

	//地面の場所
	const Vector3 ground_pos = { 0.0f, -50.0f, 0.0f };

	//フェードにかけるフレーム数
	constexpr float fade_frame = 60.0f;

	//フォントのサイズ
	constexpr int font_size = 16;

	//フォント名
	const char* font_name = "玉ねぎ楷書激無料版v7改";
}

GameScene::GameScene(SceneController& controller) :
	Scene(controller),
	m_frameCount(0)
{
}

GameScene::~GameScene()
{
	//このクラスが所有するのは複製したモデルだけなので、それらのみ削除する
	if (m_playerMHandle != -1)
		MV1DeleteModel(m_playerMHandle);
	for (auto& handle : m_enemyMHandles)
	{
		if (handle != -1)
		{
			MV1DeleteModel(handle);
		}
	}
}

void GameScene::Init()
{
	//カリングの設定
	SetUseBackCulling(true);

	//ResourceLoaderから必要なリソースを取得して初期化する
	auto& resourceLoader = ResourceLoader::GetInstance();

	//エフェクトハンドルを取得してエフェクトマネージャーを作成
	m_deathEffectHandle = resourceLoader.GetEffect(ResourceLoader::EffectID::Death);
	m_attackFieldEffectHandle = resourceLoader.GetEffect(ResourceLoader::EffectID::AttackField);
	m_pEffectManager = std::make_shared<EffectManager>(m_deathEffectHandle, m_attackFieldEffectHandle);

	//プレイヤーモデルは複製してこのシーンが所有する
	m_playerMHandle = MV1DuplicateModel(resourceLoader.GetModel(ResourceLoader::ModelID::Player));

	//地面は ResourceLoader が所有するオリジナルハンドルを利用する
	m_groundMHandle = resourceLoader.GetModel(ResourceLoader::ModelID::Ground);
	//地面の位置を設定
	MV1SetPosition(m_groundMHandle, ground_pos.ToDxLib());

	//敵のモデルは複製して EnemyFactory に渡す
	m_enemyMHandles.push_back(MV1DuplicateModel(resourceLoader.GetModel(ResourceLoader::ModelID::Zombie)));
	m_pEnemyFactory = std::make_shared<EnemyFactory>(m_enemyMHandles, m_pEffectManager);
	//敵の生成を行う工場の初期化
	m_pEnemyFactory->Init(stage_size);

	//skyboxの実体を確保
	m_pSkyBox = std::make_shared<SkyBox>(
		resourceLoader.GetGraphic(ResourceLoader::GraphicID::FrontSky),
		resourceLoader.GetGraphic(ResourceLoader::GraphicID::BackSky),
		resourceLoader.GetGraphic(ResourceLoader::GraphicID::LeftSky),
		resourceLoader.GetGraphic(ResourceLoader::GraphicID::RightSky),
		resourceLoader.GetGraphic(ResourceLoader::GraphicID::UpSky),
		resourceLoader.GetGraphic(ResourceLoader::GraphicID::DownSky));

	//プレイヤーの実体を確保
	m_pPlayer = std::make_shared<Player>(m_playerMHandle, m_pEffectManager);

	//カメラの実体を確保
	m_pCamera = std::make_shared<Camera>(m_pPlayer->GetPos());

	//当たり判定の管理クラスの実体を確保
	m_pCollManager = std::make_shared<CollisionManager>();

	//フォントハンドルを取得する
	m_fontHandle = CreateFontToHandle(font_name, font_size, -1, -1);

	//環境光だけを最大に
	SetGlobalAmbientLight(GetColorF(255, 255, 255, 255));

	//通常ライトを無効化
	SetLightEnable(TRUE);
}

void GameScene::Update(Input& input)
{
	m_frameCount++;

	//プレイヤーの更新
	m_pPlayer->Update(input, m_pCamera->GetAngleY(), GetStageSize());

	//カメラの更新
	m_pCamera->Update(m_pPlayer->GetTargetPos(), input);

	//敵すべての更新
	m_pEnemyFactory->Update(m_pPlayer->GetPos(), GetStageSize());

	//当たり判定の更新
	m_pCollManager->Update(m_pPlayer, m_pEnemyFactory);

	//エフェクトの更新
	UpdateEffekseer3D();

	//エフェクトマネージャーの更新
	m_pEffectManager->Update();

	//TODO:プレイヤーが死ぬ、または敵が全滅するなどの条件でリザルトシーンに遷移する
	//m_pEnemyFactoryから敵の数を取得する
	//m_pPlayerからプレイヤーの生死を取得する
	if (m_pEnemyFactory->GetEnemies().empty() || m_pPlayer->IsDead())
	{
		m_controller.ChangeScene(std::make_shared<ResultScene>(m_controller), fade_frame);
	}
}

void GameScene::Draw()
{
	//skyboxの描画
	m_pSkyBox->Draw(m_pCamera->GetPos());

	//地面の描画
	MV1DrawModel(m_groundMHandle);

	//グリッドの描画
	DrawGrid();

#ifdef _DEBUG
	DrawString(0, 0, "GameScene", 0xffffff);
	DrawFormatString(0, 16, 0xffffff, "FRAME:%d", m_frameCount);
#endif //DEBUG

	//オブジェクトの描画
	//プレイヤーの描画
	m_pPlayer->Draw();

	//敵すべての描画
	m_pEnemyFactory->Draw();

	//エフェクトの描画
	DrawEffekseer3D();

	//エフェクトマネージャーの描画処理を呼ぶ
	m_pEffectManager->Draw();

	//文字の描画(仮)
	//ウィンドウサイズを取得する
	auto& windowSize = Application::GetInstance().GetWindowSize();
	//表示したい文字列の横幅を取得する
	const int strWidth = GetDrawStringWidthToHandle("Time", strlen("Time"), m_fontHandle);
	Vector3 drawPos = { windowSize.w / 2.0f - strWidth / 2.0f, windowSize.h / 2.0f };
	DrawStringToHandle(static_cast<int>(drawPos.m_x), static_cast<int>(drawPos.m_y), "Time", 0xffffff, m_fontHandle);
}

void GameScene::DrawGrid()
{
	//カメラのdraw(基本デバッグ用)
	m_pCamera->Draw();

#ifdef _DEBUG
	//直線の始点と終点
	VECTOR startPos;
	VECTOR endPos;

	//ステージのサイズに合わせてグリッドを描画する
	for (int z = static_cast<int>(- stage_size.m_z);
		z <= static_cast<int>(-stage_size.m_z); z += 100)
	{
		startPos = VGet(-stage_size.m_x, 0.0f, static_cast<float>(z));
		endPos = VGet(-stage_size.m_x, 0.0f, static_cast<float>(z));
		DrawLine3D(startPos, endPos, 0xff0000);
	}
	for (int x = static_cast<int>(-stage_size.m_x); x <= static_cast<int>(-stage_size.m_x); x += 100)
	{
		startPos = VGet(static_cast<float>(x), 0.0f, -stage_size.m_z);
		endPos = VGet(static_cast<float>(x), 0.0f, -stage_size.m_z);
		DrawLine3D(startPos, endPos, 0x0000ff);
	}
#endif
}

Vector3 const GameScene::GetStageSize() const
{
	return stage_size;
}
