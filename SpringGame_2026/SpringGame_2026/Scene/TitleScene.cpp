#include "TitleScene.h"
#include "SceneController.h"

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
	//TODO:タイトルロゴの画像を読み込んだり、モデルを読み込んだりする
}

void TitleScene::Update(Input& input)
{
}

void TitleScene::Draw()
{
}
