#include "SceneMain.h"
#include <DxLib.h>
#include <cassert>
#include "../GameObjects/Player.h"
#include "../System/Input.h"
#include "../System/Camera.h"
#include "../System/CollisionManager.h"
#include "../System/EnemyFactory.h"
#include "../Graphic/BackGround.h"

namespace
{
	//カメラのnearとfar
	constexpr float camera_near = 200.0f;
	constexpr float camera_far = 1500.0f;

	//ステージのサイズ
	const Vector3 stage_size = { 1000.0f, 0.0f, 1000.0f };
}

SceneMain::SceneMain() :
	m_frameCount(0)
{
}

SceneMain::~SceneMain()
{
	//プレイヤーのコピーモデルハンドルとプレイヤーモデルハンドルを削除する
	if (m_playerCopyMHandle != -1)
		MV1DeleteModel(m_playerCopyMHandle);

	if (m_playerMHandle != -1)
		MV1DeleteModel(m_playerMHandle);

	for (int handle : m_enemyBaseMHandles)
	{
		if (handle != -1)
			MV1DeleteModel(handle);
	}

}

void SceneMain::Init()
{
	// カリングの設定
	SetUseBackCulling(true);

	// Zバッファの設定
	SetUseZBuffer3D(true);	// Zバッファを使います
	SetWriteZBuffer3D(true);// 描画する物体はZバッファにも距離を書き込む

	//モデルのロード
	m_playerMHandle = MV1LoadModel("Data/Mv1/Player.mv1");//プレイヤーのモデル
	//敵のモデルもロードする
	m_enemyBaseMHandles.push_back(MV1LoadModel("Data/Mv1/Enemy.mv1"));//敵のモデル

	//ロードに失敗した場合はアサートする
	assert(m_playerMHandle != -1);
	//敵のモデルも
	assert(m_enemyBaseMHandles[static_cast<int>(EnemyModelType::Enemy1)] != -1);

	//ロードしたモデルのハンドルをMV1DuplicateModel関数に渡して複製して、
	//複製したモデルのハンドルを渡す

	//プレイヤー
	m_playerCopyMHandle = MV1DuplicateModel(m_playerMHandle);
	m_pPlayer = std::make_shared<Player>(m_playerCopyMHandle);
	
	//敵
	m_pEnemyFactory = std::make_shared<EnemyFactory>(m_enemyBaseMHandles);

	//カメラの実体を確保
	m_pCamera = std::make_shared<Camera>(m_pPlayer->GetPos());

	//当たり判定の管理クラスの実体を確保
	m_pCollManager = std::make_shared<CollisionManager>();

	//skyboxの実体を確保
	//m_pBackGround = std::make_shared<BackGround>();

	// 環境光だけを最大に
	SetGlobalAmbientLight(GetColorF(255, 255, 255, 255));

	// 通常ライトを無効化
	SetLightEnable(TRUE);
}

void SceneMain::Update(Input& input)
{
	m_frameCount++;

	//プレイヤーの更新
	m_pPlayer->Update(input,m_pCamera->GetAngleY(),GetStageSize());

	//カメラの更新
	m_pCamera->Update(m_pPlayer->GetTargetPos(), input);

	//敵すべての更新
	m_pEnemyFactory->Update(m_pPlayer->GetPos(), GetStageSize());

	//当たり判定の更新
	m_pCollManager->Update(m_pPlayer, m_pEnemyFactory);
}

void SceneMain::Draw()
{
	////skyboxの描画
	//m_pBackGround->Draw(m_pCamera->GetPos());

	DrawGrid();

	DrawString(0,0,"SceneMain",0xffffff);
	DrawFormatString(0, 16, 0xffffff, "FRAME:%d", m_frameCount);

	//オブジェクトの描画
	//プレイヤーの描画
	m_pPlayer->Draw();

	//敵すべての描画
	m_pEnemyFactory->Draw();
}

void SceneMain::DrawGrid()
{
	//カメラのdraw(基本デバッグ用)
	m_pCamera->Draw();

#ifdef _DEBUG
	// 直線の始点と終点
	VECTOR startPos;
	VECTOR endPos;

	for (int z = -1000; z <= 1000; z += 100)
	{
		startPos = VGet(-1000.0f, 0.0f, static_cast<float>(z));
		endPos = VGet(1000.0f, 0.0f, static_cast<float>(z));
		DrawLine3D(startPos, endPos, 0xff0000);
	}
	for (int x = -1000; x <= 1000; x += 100)
	{
		startPos = VGet(static_cast<float>(x), 0.0f, -1000.0f);
		endPos = VGet(static_cast<float>(x), 0.0f, 1000.0f);
		DrawLine3D(startPos, endPos, 0x0000ff);
	}
#endif

	//塗りつぶしありの四角を描画して、地面を作る
	//四角形を三角形2つで描画する（時計回り=表面が上を向く）
	DrawTriangle3D(
		VGet(-1000.0f, 0.0f, -1000.0f),
		VGet(-1000.0f, 0.0f,  1000.0f),
		VGet( 1000.0f, 0.0f, -1000.0f),
		0x44aa44, TRUE);
	DrawTriangle3D(
		VGet( 1000.0f, 0.0f, -1000.0f),
		VGet(-1000.0f, 0.0f,  1000.0f),
		VGet( 1000.0f, 0.0f,  1000.0f),
		0x44aa44, TRUE);
}

Vector3 const SceneMain::GetStageSize() const
{
	return stage_size;
}
