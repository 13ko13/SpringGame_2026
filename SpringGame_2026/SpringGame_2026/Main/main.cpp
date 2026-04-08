#include "DxLib.h"
#include "../Constants/Game.h"
#include <memory>

#include "../Scene/SceneMain.h" 
#include "../System/Input.h"
#include "../../../Dxlib_h/EffekseerForDXLib.h"

namespace
{
	constexpr int effect_num = 12000;
}

// プログラムは WinMain から始まる
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(true); // ウインドウモードで起動
	SetMainWindowText("ゲーム名"); // ウインドウのタイトルを設定
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBitNum); // 画面サイズと色数を設定

	//エフェクトを描画するためにDirect3D11を使用するように設定（DxLib_Initの前に呼ぶ必要がある）
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	//Effekseerの初期化
	//8000は同時に描画できるエフェクトの数。これ以上描画しようとすると、古いエフェクトから順番に消えていく。
	if (Effkseer_Init(effect_num) == -1)
	{
		DxLib_End();
		return -1;
	}
	// 描画対象をバックバッファに変更
	SetDrawScreen(DX_SCREEN_BACK);

	auto pScene = std::make_shared<SceneMain>();
	pScene->Init();
	Input input;//入力のためのオブジェクト

	while (ProcessMessage() != -1)
	{
		LONGLONG start = GetNowHiPerformanceCount(); // フレーム開始時間を取得
		ClearDrawScreen(); // 画面をクリア

		pScene->Update(input);
		pScene->Draw();

		//インプットの更新処理
		input.Update();

		// escキーで終了
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}
		ScreenFlip(); // 描画した内容を画面に反映する
		while (GetNowHiPerformanceCount() - start < 16667) {} // 約16.667ミリ秒(1/60秒)待つことで60FPSに固定
	}

	//シーンを先に破棄する（DxLib/Effekseerのリソースを解放するため）
	pScene.reset();

	Effkseer_End();				// Effekseerの終了処理
	DxLib_End();				// ＤＸライブラリ使用の終了処理
	return 0;				// ソフトの終了 
}