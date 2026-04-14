#include "TitleScene.h"
#include "SceneController.h"
#include <cassert>
#include "../System/Input.h"

namespace
{
	//地面の場所
	const Vector3 ground_pos = { 0.0f, -50.0f, 0.0f };
}

TitleScene::TitleScene(SceneController& controller):
	Scene(controller)
{
}

TitleScene::~TitleScene()
{
	//TODO:タイトルロゴの画像を削除したり、モデルを削除したりする

	//SceneControllerの初期化を行う
	m_controller.Init();
}

void TitleScene::Init()
{
	//地面のモデルを読み込む
	int handle = MV1LoadModel("Data/Mv1/Ground.mv1");
	//ロードに失敗した場合はアサートする
	assert(handle != -1);
	//モデルのハンドルを配列に入れる
	m_modelHandles.push_back(handle);
	MV1SetPosition(m_modelHandles[static_cast<int>(ModelType::Ground)], ground_pos.ToDxLib());

	//カメラの実体を確保
	//まだプレイヤーがいないので、注視点の位置を(0,0,0)にしておく
	m_pCamera = std::make_shared<Camera>(Vector3(0.0f, 0.0f, 0.0f));

	//TODO:タイトルロゴの画像を読み込んだり、プレイヤーモデルを読み込んだりする
}

void TitleScene::Update(Input& input)
{
	//カメラの更新
	//まだプレイヤーがいないので、注視点の位置を(0,0,0)にしておく
	m_pCamera->Update(Vector3(0.0f, 0.0f, 0.0f), input);
}

void TitleScene::Draw()
{
	//地面のモデルを描画する
	MV1DrawModel(m_modelHandles[static_cast<int>(ModelType::Ground)]);

	//カメラの描画
	m_pCamera->Draw();

	//TODO:タイトルロゴの画像を描画したり、モデルを描画したりする
}
