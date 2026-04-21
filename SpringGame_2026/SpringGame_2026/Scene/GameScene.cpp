#define NOMINMAX

#include <DxLib.h>
#include <cassert>
#include <EffekseerForDXLib.h>
#include <string>
#include <cmath>
#include <algorithm>

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
#include "../System/ToKanji.h"
#include "../Manager/SoundManager.h"

namespace
{
	//カメラのnearとfar
	constexpr float camera_near = 200.0f;
	constexpr float camera_far = 1800.0f;

	//ステージのサイズ
	const Vector3 stage_size = { 1400.0f, 0.0f, 1400.0f };

	//地面の場所
	const Vector3 ground_pos = { 0.0f, -50.0f, 0.0f };

	//フェードにかけるフレーム数
	constexpr float fade_frame = 60.0f;
	//リザルトへの遷移にかけるフレーム数
	constexpr float to_result_fade_frame = 120.0f;

	//フォントのサイズ
	constexpr int time_limit_font_size = 32;//残り時間のフォントサイズ
	constexpr int game_count_font_size = 200;//ゲーム開始前のカウントダウンのフォントサイズ

	//フォント名
	const char* font_name = "玉ねぎ楷書激無料版v7改";

	//「残り時間」の文字列の位置を割合で指定
	constexpr float time_text_pos_x_rate = 0.97f;
	constexpr float time_text_pos_y_rate = 0.05f;

	//「残り敵の数」の文字列の位置を割合で指定
	constexpr float e_num_text_pos_x_rate = 0.97f;
	constexpr float e_num_text_pos_y_rate = 0.12f;

	//1秒あたりのフレーム数
	constexpr int frame_per_second = 60;

	//ゲーム開始時のカウントダウンの時間
	constexpr int start_count_down_time = frame_per_second * 4;//4秒

	//敵を倒したときのスコア倍率
	constexpr int score_rate = 3;

	//残り時間に応じたスコアの減少割合
	constexpr float time_score_rate = 0.3f;

	//敵を倒すまでの時間の最大値
	constexpr int max_time_for_kill = frame_per_second * 99;//99秒

	//最大スコア
	constexpr int max_score = 100;

	//最大スコアを目指せる目安の時間23秒
	constexpr int ideal_time = frame_per_second * 23;

	//敵を倒したときの割合の重み
	constexpr float kill_waight = 0.7f;
	//時間に対する割合の重み
	constexpr float time_waight = 0.3f;
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

	//ゲーム開始時のカウントダウンの時間を設定
	m_startCountDown = start_count_down_time;//3秒

	//ResourceLoaderから必要なリソースを取得して初期化する
	auto& resourceLoader = ResourceLoader::GetInstance();

	//エフェクトハンドルを取得してエフェクトマネージャーを作成
	m_deathEffectHandle = resourceLoader.GetEffect(ResourceLoader::EffectID::Death);
	m_attackFieldEffectHandle = resourceLoader.GetEffect(ResourceLoader::EffectID::AttackField);
	m_pEffectManager = std::make_shared<EffectManager>(m_deathEffectHandle, m_attackFieldEffectHandle);

	//念のため全てのエフェクトを停止しておく
	m_pEffectManager->StopAllEffects();

	//プレイヤーモデルは複製してこのシーンが所有する
	m_playerMHandle = MV1DuplicateModel(resourceLoader.GetModel(ResourceLoader::ModelID::Player));

	//地面は ResourceLoader が所有するオリジナルハンドルを利用する
	m_groundMHandle = resourceLoader.GetModel(ResourceLoader::ModelID::Ground);
	//地面の位置を設定
	MV1SetPosition(m_groundMHandle, ground_pos.ToDxLib());

	//敵のモデルは複製して EnemyFactory に渡す
	m_enemyMHandles.push_back(MV1DuplicateModel(resourceLoader.GetModel(ResourceLoader::ModelID::Zombie)));
	m_pEnemyFactory = std::make_shared<EnemyFactory>(m_enemyMHandles, m_pEffectManager);

	//プレイヤーの実体を確保
	m_pPlayer = std::make_shared<Player>(m_playerMHandle, m_pEffectManager);

	//敵の生成を行う工場の初期化
	m_pEnemyFactory->Init(stage_size, m_pPlayer->GetPos());

	//敵の数を保持
	m_enemyCount = static_cast<int>(m_pEnemyFactory->GetEnemies().size());

	//skyboxの実体を確保
	m_pSkyBox = std::make_shared<SkyBox>(
		resourceLoader.GetGraphic(ResourceLoader::GraphicID::FrontSky),
		resourceLoader.GetGraphic(ResourceLoader::GraphicID::BackSky),
		resourceLoader.GetGraphic(ResourceLoader::GraphicID::LeftSky),
		resourceLoader.GetGraphic(ResourceLoader::GraphicID::RightSky),
		resourceLoader.GetGraphic(ResourceLoader::GraphicID::UpSky),
		resourceLoader.GetGraphic(ResourceLoader::GraphicID::DownSky));

	//カメラの実体を確保
	m_pCamera = std::make_shared<Camera>(m_pPlayer->GetPos());

	//当たり判定の管理クラスの実体を確保
	m_pCollManager = std::make_shared<CollisionManager>(m_pEffectManager);

	//フォントハンドルを取得する
	m_timeFontHandle = CreateFontToHandle(font_name, time_limit_font_size, -1, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	m_countDownFontHandle = CreateFontToHandle(font_name, game_count_font_size, -1, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);

	//ゲームBGMを再生する
	SoundManager::GetInstance().PlayFadeIn(SoundManager::SoundType::GameBgm, fade_frame, true);

	//環境光だけを最大に
	SetGlobalAmbientLight(GetColorF(255, 255, 255, 255));

	//通常ライトを無効化
	SetLightEnable(TRUE);
}

void GameScene::Update(Input& input)
{
	//フレームカウンターの更新
	m_frameCount++;

	//ゲーム開始前のカウントダウン
	//時間が3、2、1のときにカウントダウンの音を鳴らす
	if (m_startCountDown % frame_per_second == 0 && m_startCountDown >= frame_per_second * 2)
	{
		//カウントダウンの音を鳴らす
		SoundManager::GetInstance().Play(SoundManager::SoundType::CountDown);
	}
	else if (m_startCountDown == frame_per_second)
	{
		//開始時の音を鳴らす
		SoundManager::GetInstance().Play(SoundManager::SoundType::Start);
	}

	//カウントダウンの更新
	m_startCountDown--;


	//カメラの更新
	m_pCamera->Update(m_pPlayer->GetTargetPos(), input);

	//カウントダウン中はプレイヤーと敵は待機モーションのみ再生させる
	if (m_startCountDown >= 0)
	{
		//プレイヤーの更新
		m_pPlayer->Update(input, m_pCamera->GetAngleY(), stage_size, false);
		//敵すべての更新
		m_pEnemyFactory->Update(m_pPlayer->GetPos(), GetStageSize(), false);
	}
	else
	{
		//カウントダウンが終了したら通常の更新処理を行う
		//プレイヤーの更新
		m_pPlayer->Update(input, m_pCamera->GetAngleY(), stage_size, true);
		//敵すべての更新
		m_pEnemyFactory->Update(m_pPlayer->GetPos(), GetStageSize(), true);
	}

	//エフェクトの更新
	UpdateEffekseer3D();

	//カウントダウン中はゲーム時間の更新処理を行わない
	if (m_startCountDown >= 60) return;

	//ゲーム時間の更新
	//プレイヤーに討伐時間を競わせるためにカウントアップ制にする
	//敵が生きていたら、更新処理を行う
	if (!m_pEnemyFactory->GetEnemies().empty()) m_time++;

	//当たり判定の更新
	m_pCollManager->Update(m_pPlayer, m_pEnemyFactory);

	//エフェクトマネージャーの更新
	m_pEffectManager->Update();

	//プレイヤーが死ぬ、または敵が全滅するなどの条件でリザルトシーンに遷移する
	//m_pEnemyFactoryから敵の数を取得する
	//m_pPlayerからプレイヤーの生死を取得する
	if (m_pEnemyFactory->GetEnemies().empty() || m_pPlayer->IsDead())
	{
		//スコアの計算を行う
		int score = CalcScore(m_enemyCount - static_cast<int>(m_pEnemyFactory->GetEnemies().size()));
		m_controller.ChangeScene(std::make_shared<ResultScene>(m_controller, score), to_result_fade_frame);

		//ゲームBGMをフェードアウトさせる
		SoundManager::GetInstance().FadeOut(SoundManager::SoundType::GameBgm, to_result_fade_frame);
	}
#ifdef _DEBUG
	if (input.IsTriggered("ok"))
	{
		//スコアの計算を行う
		int score = CalcScore(m_enemyCount - static_cast<int>(m_pEnemyFactory->GetEnemies().size()));
		m_controller.ChangeScene(std::make_shared<ResultScene>(m_controller, score), to_result_fade_frame);

		//ゲームBGMをフェードアウトさせる
		SoundManager::GetInstance().FadeOut(SoundManager::SoundType::GameBgm, to_result_fade_frame);
	}
#endif // _DEBUG	
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
	DrawFormatString(0, 64, 0xffffff, "Time:%d", m_time / 60);
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

	//残り時間の描画
	//stringで描画する文字列を作成する
	std::string timeText = ToKanji::NumToKanji(m_time / 60) + "秒";//漢字に変換して「秒」をつける

	//表示したい文字列の横幅を取得する
	const int strWidth = GetDrawStringWidthToHandle("残り時間:60拍", static_cast<int>(strlen("残り時間:60拍")), m_timeFontHandle);
	//右上に描画するため、描画位置を計算する
	Vector3 drawPos = {
		static_cast<float>(windowSize.w * time_text_pos_x_rate - strWidth) ,
		static_cast<float>(windowSize.h * time_text_pos_y_rate) };
	DrawStringToHandle(static_cast<int>(drawPos.m_x),
		static_cast<int>(drawPos.m_y),
		timeText.c_str(),
		0xffffff, m_timeFontHandle);

	//残りの敵の数を描画する
	//まず敵の数を取得する
	int enemyCount = static_cast<int>(m_pEnemyFactory->GetEnemies().size());
	//stringで描画する文字列を作成する
	std::string enemyText = "残り敵数:" + ToKanji::NumToKanji(enemyCount) + "体";//漢字に変換して「体」をつける
	//敵の数が1桁になったときに文字列の位置が変わらないように、最大桁数のときの文字列の横幅を取得しておく
	std::string maxEnemyText = "残り敵数:九十九体";
	const int strMaxWidth = GetDrawStringWidthToHandle(maxEnemyText.c_str(), static_cast<int>(strlen(maxEnemyText.c_str())), m_timeFontHandle);
	//ウィンドウの右上に描画するため、描画位置を計算する
	drawPos = { static_cast<float>(windowSize.w * e_num_text_pos_x_rate - strMaxWidth),
		static_cast<float>(windowSize.h * e_num_text_pos_y_rate) };
	DrawStringToHandle(static_cast<int>(drawPos.m_x),
		static_cast<int>(drawPos.m_y),
		enemyText.c_str(),
		0xffffff, m_timeFontHandle);

	//ゲーム開始時のカウントダウンの描画
	if (m_startCountDown >= 60)
	{
		//stringで描画する文字列を作成する
		std::string startTimeText = ToKanji::NumToKanji(m_startCountDown / 60);

		//表示したい文字列の横幅を取得する
		const int strWidth = GetDrawStringWidthToHandle(startTimeText.c_str(), static_cast<int>(strlen(startTimeText.c_str())), m_countDownFontHandle);
		//表示したい文字の高さ
		const int strHeight = game_count_font_size;

		//ウィンドウの中心に描画するため、描画位置を計算する
		Vector3 drawPos = { windowSize.w / 2.0f - strWidth / 2.0f, windowSize.h / 2.0f - strHeight / 2.0f };
		DrawExtendStringToHandle(static_cast<int>(drawPos.m_x), static_cast<int>(drawPos.m_y),
			1.0, 1.0, startTimeText.c_str(), 0xffffff, m_countDownFontHandle);
	}
	else if (m_startCountDown > 0)
	{
		//stringで描画する文字列を作成する
		std::string startTimeText = "始めいッ!";
		//表示したい文字列の横幅を取得する
		const int strWidth = GetDrawStringWidthToHandle(startTimeText.c_str(), static_cast<int>(strlen(startTimeText.c_str())), m_countDownFontHandle);
		//表示したい文字の高さ
		const int strHeight = game_count_font_size;
		//ウィンドウの中心に描画するため、描画位置を計算する
		Vector3 drawPos = { windowSize.w / 2.0f - strWidth / 2.0f, windowSize.h / 2.0f - strHeight / 2.0f };
		DrawExtendStringToHandle(static_cast<int>(drawPos.m_x), static_cast<int>(drawPos.m_y),
			1.0, 1.0, startTimeText.c_str(), 0xffffff, m_countDownFontHandle);
	}
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
	for (int z = static_cast<int>(-stage_size.m_z);
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

int GameScene::CalcScore(int deadEnemyNum) const
{
	//スコア
	int score = 0;

	//スコアは0～100に正規化して表示する
	//敵を倒した割合
	float killRate = static_cast<float>(deadEnemyNum) / static_cast<float>(m_enemyCount);
	//どのくらい時間がかかっているか
	float elapsedTime = static_cast<float>(std::min(m_time, max_time_for_kill));

	float timeRate = 0.0f;

	// 目標時間内なら満点、それ以降は線形で減少して max_time_for_kill で 0 にする
	if (elapsedTime <= static_cast<float>(ideal_time)) {
		timeRate = 1.0f;
	}
	else {
		float denom = static_cast<float>(max_time_for_kill - ideal_time);
		if (denom <= 0.0f) {
			timeRate = 0.0f;
		}
		else {
			timeRate = std::clamp(1.0f - (elapsedTime - static_cast<float>(ideal_time)) / denom, 0.0f, 1.0f);
		}
	}

	//割合に重みをかけて合計する 
	float normalizedScore = kill_waight * killRate + time_waight * timeRate;
	//最大スコアをかけて、整数に丸めて、0～max_scoreの範囲に収める
	score = std::clamp(static_cast<int>(std::round(normalizedScore * max_score)), 0, max_score);

	return score;
}

Vector3 const GameScene::GetStageSize() const
{
	return stage_size;
}
