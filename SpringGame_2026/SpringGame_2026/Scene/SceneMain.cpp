#include "SceneMain.h"
#include <DxLib.h>
#include <cassert>
#include "../GameObjects/Player.h"
#include "../System/Input.h"
#include "../System/Camera.h"
#include "../GameObjects/Enemy.h"

namespace
{
	//カメラのnearとfar
	constexpr float camera_near = 200.0f;
	constexpr float camera_far = 1500.0f;
}

SceneMain::SceneMain() :
	m_frameCount(0)
{
}

SceneMain::~SceneMain()
{
	//モデルのコピーの方のハンドルを先に開放する
	for(int handle : m_modelCopyHandles)
	{
		MV1DeleteModel(handle);
	}

	//モデルのハンドルを開放する
	for(int handle : m_modelBaseHandles)
	{
		MV1DeleteModel(handle);
	}
}

void SceneMain::Init()
{
	// カリングの設定
	SetUseBackCulling(true);

	// Zバッファの設定
	SetUseZBuffer3D(true);	// Zバッファを使います
	SetWriteZBuffer3D(true);	// 描画する物体はZバッファにも距離を書き込む

	//モデルのロード
	m_modelBaseHandles.push_back(MV1LoadModel("Data/PlayerCopy.mv1"));//プレイヤーのモデル
	//敵のモデルもロードする
	m_modelBaseHandles.push_back(MV1LoadModel("Data/Enemy.mv1"));//敵のモデル

	//ロードに失敗した場合はアサートする
	assert(m_modelBaseHandles[static_cast<int>(ModelType::Player)] != -1);
	//敵のモデルも
	assert(m_modelBaseHandles[static_cast<int>(ModelType::Enemy)] != -1);

	//ロードしたモデルのハンドルをMV1DuplicateModel関数に渡して複製して、
	//複製したモデルのハンドルを渡す
	auto temp = MV1DuplicateModel(m_modelBaseHandles[static_cast<int>(ModelType::Player)]);
	m_modelCopyHandles.push_back(MV1DuplicateModel(m_modelBaseHandles[static_cast<int>(ModelType::Player)]));
	m_pPlayer = std::make_shared<Player>(m_modelCopyHandles[static_cast<int>(ModelType::Player)]);

	temp = MV1DuplicateModel(m_modelBaseHandles[static_cast<int>(ModelType::Enemy)]);
	m_modelCopyHandles.push_back(MV1DuplicateModel(m_modelBaseHandles[static_cast<int>(ModelType::Enemy)]));
	m_pEnemy = std::make_shared<Enemy>(m_modelCopyHandles[static_cast<int>(ModelType::Enemy)]);

	//カメラの実体を確保
	m_pCamera = std::make_shared<Camera>(m_pPlayer->GetPos());


	// 環境光だけを最大に
	SetGlobalAmbientLight(GetColorF(255, 255, 255, 255));

	// 通常ライトを無効化
	SetLightEnable(TRUE);
}

void SceneMain::Update(Input& input)
{
	m_frameCount++;

	//プレイヤーの更新
	m_pPlayer->Update(input,m_pCamera->GetAngleY());

	//敵の更新
	m_pEnemy->Update();

	//カメラの更新
	m_pCamera->Update(m_pPlayer->GetTargetPos(), input);
}

void SceneMain::Draw()
{
	DrawGrid();

	DrawString(0,0,"SceneMain",0xffffff);
	DrawFormatString(0, 16, 0xffffff, "FRAME:%d", m_frameCount);

	//オブジェクトの描画
	//プレイヤーの描画
	m_pPlayer->Draw();

	//敵の描画
	m_pEnemy->Draw();
}

void SceneMain::DrawGrid()
{
	// 直線の始点と終点
	VECTOR startPos;
	VECTOR endPos;

	//カメラのdraw(基本デバッグ用)
	m_pCamera->Draw();

	for (int z = -300; z <= 300; z += 100)
	{
		startPos = VGet(-300.0f, 0.0f, static_cast<float>(z));
		endPos = VGet(300.0f, 0.0f, static_cast<float>(z));
		DrawLine3D(startPos, endPos, 0xff0000);
	}
	for (int x = -300; x <= 300; x += 100)
	{
		startPos = VGet(static_cast<float>(x), 0.0f, -300.0f);
		endPos = VGet(static_cast<float>(x), 0.0f, 300.0f);
		DrawLine3D(startPos, endPos, 0x0000ff);
	}
}
