#pragma once
#include "Scene.h"
#include "../System/SpriteAnimator.h"
#include "../System/ModelAnimator.h"
#include "../System/Camera.h"

class TitleScene : public Scene
{
public:
	TitleScene(SceneController& controller);
	~TitleScene();

	void Init() override;
	void Update(Input& input) override;
	void Draw() override;

private:
	enum class ModelType : int
	{
		Ground = 0,//地面
	};

	//グラフィックハンドルの配列
	std::vector<int> m_graphHandles;
	//モデルハンドルの配列
	std::vector<int> m_modelHandles;
	//タイトルのアニメーション
	SpriteAnimator m_titleAnim;

	//カメラの実体
	std::shared_ptr<Camera> m_pCamera;
};