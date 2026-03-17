#include "SceneMain.h"
#include <DxLib.h>
#include <cassert>
#include "Player.h"

SceneMain::SceneMain() :
	m_frameCount(0)
{
}

SceneMain::~SceneMain()
{
}

void SceneMain::Init()
{
	// カリングの設定
	SetUseBackCulling(true);

	// Zバッファの設定
	SetUseZBuffer3D(true);	// Zバッファを使います
	SetWriteZBuffer3D(true);	// 描画する物体はZバッファにも距離を書き込む

	// カメラの設定
	SetCameraPositionAndTarget_UpVecY(VGet(0.0f, 300.0f, -700.0f), VGet(0.0f, 0.0f, 0.0f));
	SetupCamera_Perspective(DX_PI_F / 3.0f);
	SetCameraNearFar(200.0f, 1500.0f);

	//モデルのロード
	m_modelBaseHandles.push_back(MV1LoadModel("Data/Player.mv1"));//プレイヤーのモデル
	//TODO:敵のモデルもロードする

	//ロードに失敗した場合はアサートする
	assert(m_modelBaseHandles[static_cast<int>(ModelType::Player)] != -1);
	//TODO:敵のモデルも

	//ロードしたモデルのハンドルをMV1DuplicateModel関数に渡して複製して、
	//複製したモデルのハンドルを渡す
	auto temp = MV1DuplicateModel(m_modelBaseHandles[static_cast<int>(ModelType::Player)]);
	m_modelCopyHandles.push_back(MV1DuplicateModel(m_modelBaseHandles[static_cast<int>(ModelType::Player)]));
	m_pPlayer = std::make_shared<Player>(m_modelCopyHandles[static_cast<int>(ModelType::Player)]);
}

void SceneMain::Update()
{
	m_frameCount++;

	//プレイヤーの更新
	m_pPlayer->Update();
}

void SceneMain::Draw()
{
	DrawGrid();

	DrawString(0,0,"SceneMain",0xffffff);
	DrawFormatString(0, 16, 0xffffff, "FRAME:%d", m_frameCount);

	//オブジェクトの描画
	//プレイヤーの描画
	m_pPlayer->Draw();
}

void SceneMain::DrawGrid()
{
	// 直線の始点と終点
	VECTOR startPos;
	VECTOR endPos;

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
