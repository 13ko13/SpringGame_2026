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
#include "../UI/TextUI.h"

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
	constexpr float to_result_fade_frame = 80.0f;

	//フォントのサイズ
	constexpr int time_limit_font_size = 32;//残り時間のフォントサイズ
	constexpr int game_count_font_size = 200;//ゲーム開始前のカウントダウンのフォントサイズ
	constexpr int m_enemyCountFontSize = 32;//残り敵の数のフォントサイズ

	//フォント名
	const char* font_name = "玉ねぎ楷書激無料版v7改";

	//「残り時間」の文字列の位置を割合で指定
	constexpr float time_text_pos_x_rate = 0.5f;
	constexpr float time_text_pos_y_rate = 0.05f;

	//「残り敵の数」の文字列の位置を割合で指定
	constexpr float e_num_text_pos_x_rate = 0.5f;
	constexpr float e_num_text_pos_y_rate = 0.1f;

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

	//討伐完了フォントのサイズ
	constexpr int clear_font_size = 240;
	//討伐完了の1文字を出すときのフレーム数
	constexpr int clear_shrink_frame = 50;
	//討伐完了の最初の大きさ
	constexpr float clear_start_scale = 4.0f;
	//討伐完了の縁のサイズ
	constexpr int clear_font_edge_size = 20;

	//カメラのターゲットの位置
	const Vector3 camera_target_pos = { 0.0f, 400.0f, 0.0f };

	//操作方法フォントのサイズ
	constexpr int how_to_play_font_size = 24;

	//スティックで移動フォントを描画する画面位置割合
	constexpr float operation_move_text_pos_x_rate = 0.85f;
	constexpr float operation_move_text_pos_y_rate = 0.85f;

	//Yボタンで攻撃フォントを描画する画面位置割合
	constexpr float operation_attack_text_pos_x_rate = 0.85f;
	constexpr float operation_attack_text_pos_y_rate = 0.9f;
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

	//カメラの実体を確保
	m_pCamera = std::make_shared<Camera>(camera_target_pos);

	//プレイヤーの実体を確保
	m_pPlayer = std::make_shared<Player>(m_playerMHandle, m_pEffectManager, m_pCamera);

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

	//当たり判定の管理クラスの実体を確保
	m_pCollManager = std::make_shared<CollisionManager>(m_pEffectManager);

	//フォントハンドルを取得する
	int fontType = DX_FONTTYPE_ANTIALIASING_EDGE_4X4;
	m_timeFontHandle = CreateFontToHandle(font_name, time_limit_font_size, -1, fontType);
	m_countDownFontHandle = CreateFontToHandle(font_name, game_count_font_size, -1, fontType);
	m_enemyCountFontHandle = CreateFontToHandle(font_name, m_enemyCountFontSize, -1, fontType);
	m_clearFontHandle = CreateFontToHandle(font_name, clear_font_size, -1, fontType, -1, clear_font_edge_size);
	m_operationFontHandle = CreateFontToHandle(font_name, how_to_play_font_size, -1, fontType);

	//テキストUIの実体を確保
	m_pTimeText = std::make_shared<TextUI>(m_timeFontHandle, time_limit_font_size);
	m_pCountDownText = std::make_shared<TextUI>(m_countDownFontHandle, game_count_font_size);
	m_pEnemyCountText = std::make_shared<TextUI>(m_enemyCountFontHandle, m_enemyCountFontSize);
	m_pClearText = std::make_shared<TextUI>(m_clearFontHandle, clear_font_size);
	m_pOperationText = std::make_shared<TextUI>(m_operationFontHandle, how_to_play_font_size);

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
	//フェード中でなければ時間が3、2、1のときにカウントダウンの音を鳴らす
	if (m_startCountDown % frame_per_second == 0 &&
		m_startCountDown >= frame_per_second * 2 &&
		!m_controller.GetFade().IsFading())
	{
		//カウントダウンの音を鳴らす
		SoundManager::GetInstance().Play(SoundManager::SoundType::CountDown);
	}
	else if (m_startCountDown == frame_per_second)
	{
		//開始時の音を鳴らす
		SoundManager::GetInstance().Play(SoundManager::SoundType::Start);
	}


	//フェードが終わっていればカウントダウンの更新を行う
	if (!m_controller.GetFade().IsFading())
	{
		//カウントダウンの更新
		m_startCountDown--;
	}

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

	if (m_pEnemyFactory->GetEnemies().empty())
	{
		m_clearFrame++;
	}

	//プレイヤーが死ぬ、または敵が全滅するなどの条件でリザルトシーンに遷移する
	//m_pEnemyFactoryから敵の数を取得する
	//m_pPlayerからプレイヤーの生死を取得する
	if (m_isEndRyou)
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

	//時間の描画
	DrawTime();

	//敵の数の描画
	DrawEnemyCount();

	//カウントダウンの描画
	DrawCountDown();

	//敵がすべて倒されたときの「討伐完了」の描画
	if (m_pEnemyFactory->GetEnemies().empty())
	{
		//文字のサイズをt秒かけてn倍の大きさから当倍に(大→小)にしてスタンプのように描画する
		float scale = (1.0f - static_cast<float>(m_clearFrame) / clear_shrink_frame) * clear_start_scale;
		if (!m_isEndTou)
		{
			std::string clearText = "討";
			m_pClearText->DrawAtRate(clearText, 0.2f, 0.5f, 0xff0000, scale, scale, 0x000000);
			if (scale < 1.0f)
			{
				scale = 1.0f;
				m_isEndTou = true;
				m_clearFrame = 0;//フレーム数をリセットして次の文字の描画に備える
			}
		}
		else
		{
			scale = 1.0f;
			std::string clearText = "討";
			m_pClearText->DrawAtRate(clearText, 0.2f, 0.5f, 0xff0000, scale, scale, 0x000000);
			//討のSEを鳴らす
			if (!m_isTouSE)
			{
				SoundManager::GetInstance().Play(SoundManager::SoundType::ScorePop);
				m_isTouSE = true;
			}
		}
	}
	//敵がすべて倒されたときの「討伐完了」の描画
	if (m_pEnemyFactory->GetEnemies().empty() &&
			m_isEndTou)
	{
		//文字のサイズをt秒かけてn倍の大きさから当倍に(大→小)にしてスタンプのように描画する
		float scale = (1.0f - static_cast<float>(m_clearFrame) / clear_shrink_frame) * clear_start_scale;
		if (!m_isEndBatsu)
		{
			std::string clearText = "伐";
			m_pClearText->DrawAtRate(clearText, 0.4f, 0.5f, 0xff0000, scale, scale, 0x000000);
			if (scale < 1.0f)
			{
				scale = 1.0f;
				m_isEndBatsu = true;
				m_clearFrame = 0;//フレーム数をリセットして次の文字の描画に備える
			}
		}
		else
		{
			scale = 1.0f;
			std::string clearText = "伐";
			m_pClearText->DrawAtRate(clearText, 0.4f, 0.5f, 0xff0000, scale, scale, 0x000000);
			//伐のSEを鳴らす
			if (!m_isBatsuSE)
			{
				SoundManager::GetInstance().Play(SoundManager::SoundType::ScorePop);
				m_isBatsuSE = true;
			}
		}
	}
	//敵がすべて倒されたときの「討伐完了」の描画
	if (m_pEnemyFactory->GetEnemies().empty() &&
			m_isEndBatsu)
	{
		//文字のサイズをt秒かけてn倍の大きさから当倍に(大→小)にしてスタンプのように描画する
		float scale = (1.0f - static_cast<float>(m_clearFrame) / clear_shrink_frame) * clear_start_scale;
		if (!m_isEndKan)
		{
			std::string clearText = "完";
			m_pClearText->DrawAtRate(clearText, 0.6f, 0.5f, 0xff0000, scale, scale, 0x000000);
			if (scale < 1.0f)
			{
				scale = 1.0f;
				m_isEndKan = true;
				m_clearFrame = 0;//フレーム数をリセットして次の文字の描画に備える
			}
		}
		else
		{
			scale = 1.0f;
			std::string clearText = "完";
			m_pClearText->DrawAtRate(clearText, 0.6f, 0.5f, 0xff0000, scale, scale, 0x000000);
			//完のSEを鳴らす
			if (!m_isKanSE)
			{
				SoundManager::GetInstance().Play(SoundManager::SoundType::ScorePop);
				m_isKanSE = true;
			}
		}
	}
	//敵がすべて倒されたときの「討伐完了」の描画
	if (m_pEnemyFactory->GetEnemies().empty() &&
			m_isEndKan)
	{
		//文字のサイズをt秒かけてn倍の大きさから当倍に(大→小)にしてスタンプのように描画する
		float scale = (1.0f - static_cast<float>(m_clearFrame) / clear_shrink_frame) * clear_start_scale;
		if (!m_isEndRyou)
		{
			std::string clearText = "了";
			m_pClearText->DrawAtRate(clearText, 0.8f, 0.5f, 0xff0000, scale, scale, 0x000000);
			if (scale < 1.0f)
			{
				scale = 1.0f;
				m_isEndRyou = true;
				m_clearFrame = 0;//フレーム数をリセットして次の文字の描画に備える
			}
		}
		else
		{
			scale = 1.0f;
			std::string clearText = "了";
			m_pClearText->DrawAtRate(clearText, 0.8f, 0.5f, 0xff0000, scale, scale, 0x000000);
			//了のSEを鳴らす
			if (!m_isRyouSE)
			{
				SoundManager::GetInstance().Play(SoundManager::SoundType::ScorePop);
				m_isRyouSE = true;
			}
		}
	}

	//右下に操作方法を描画する
	std::string operationText = "Lスティック:移動";
	m_pOperationText->DrawAtRate(operationText, operation_move_text_pos_x_rate, operation_move_text_pos_y_rate, 0x000000, 1.0, 1.0, 0xffffff);
	operationText = "X:攻撃";
	m_pOperationText->DrawAtRate(operationText, operation_attack_text_pos_x_rate, operation_attack_text_pos_y_rate, 0x000000, 1.0, 1.0, 0xffffff);
	
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

void GameScene::DrawTime() const
{
	//残り時間の描画
	//stringで描画する文字列を作成する
	std::string timeText = ToKanji::NumToKanji(m_time / 60) + "秒";//漢字に変換して「秒」をつける
	//ウィンドウの上部中央に描画するため、TextUIのDrawAtRateを使用する
	m_pTimeText->DrawAtRate(timeText, time_text_pos_x_rate, time_text_pos_y_rate, 0x000000, 1.0, 1.0, 0xffffff);
}

void GameScene::DrawEnemyCount() const
{
	//残りの敵の数を描画する
	//まず敵の数を取得する
	int enemyCount = static_cast<int>(m_pEnemyFactory->GetEnemies().size());
	//stringで描画する文字列を作成する
	std::string enemyText = "残り敵数:" + ToKanji::NumToKanji(enemyCount) + "体";//漢字に変換して「体」をつける
	//ウィンドウの上部中央に描画するため、TextUIのDrawAtRateを使用する
	m_pEnemyCountText->DrawAtRate(enemyText, e_num_text_pos_x_rate, e_num_text_pos_y_rate, 0x000000, 1.0, 1.0, 0xffffff);
}

void GameScene::DrawCountDown() const
{
	//ゲーム開始時のカウントダウンの描画
	//4秒の時は描画を行わない
	if (m_startCountDown >= 60 &&
		m_startCountDown < start_count_down_time)
	{
		//stringで描画する文字列を作成する
		std::string startTimeText = ToKanji::NumToKanji(m_startCountDown / 60);
		//ウィンドウの中央に描画するため、TextUIのDrawCenterを使用する
		m_pCountDownText->DrawCenter(startTimeText, 0x000000, 1.0, 1.0, 0xffffff);
	}
	else if (m_startCountDown > 0 &&
			m_startCountDown < start_count_down_time)
	{
		//stringで描画する文字列を作成する
		std::string startTimeText = "始めいッ!";
		//ウィンドウの中央に描画するため、TextUIのDrawCenterを使用する
		m_pCountDownText->DrawCenter(startTimeText, 0x000000, 1.0, 1.0, 0xffffff);
	}
}

Vector3 const GameScene::GetStageSize() const
{
	return stage_size;
}
