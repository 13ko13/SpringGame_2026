#include "SceneMain.h"
#include <DxLib.h>
#include <cassert>
#include "../GameObjects/Player.h"
#include "../System/Input.h"
#include "../System/Camera.h"

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
	m_modelBaseHandles.push_back(MV1LoadModel("Data/PlayerCopy.mv1"));//プレイヤーのモデル
	//TODO:敵のモデルもロードする

	//ロードに失敗した場合はアサートする
	assert(m_modelBaseHandles[static_cast<int>(ModelType::Player)] != -1);
	//TODO:敵のモデルも

	//ロードしたモデルのハンドルをMV1DuplicateModel関数に渡して複製して、
	//複製したモデルのハンドルを渡す
	auto temp = MV1DuplicateModel(m_modelBaseHandles[static_cast<int>(ModelType::Player)]);
	m_modelCopyHandles.push_back(MV1DuplicateModel(m_modelBaseHandles[static_cast<int>(ModelType::Player)]));
	m_pPlayer = std::make_shared<Player>(m_modelCopyHandles[static_cast<int>(ModelType::Player)]);

	//カメラの実体を確保
	m_pCamera = std::make_shared<Camera>(m_pPlayer->GetPos());


	// 環境光だけを最大に
	SetGlobalAmbientLight(GetColorF(255, 255, 255, 255));

	// 通常ライトを無効化
	SetLightEnable(FALSE);

}

void SceneMain::Update(Input& input)
{
	m_frameCount++;

	//プレイヤーの更新
	m_pPlayer->Update(input,m_pCamera->GetAngleY());

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
