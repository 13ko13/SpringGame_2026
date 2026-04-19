#pragma once
#include <vector>
#include <memory>
#include "../Math/Vector3.h"
#include "Scene.h"

//前方宣言
class Player;
class Input;
class Camera;
class Enemy;
class CollisionManager;
class EnemyFactory;
class SkyBox;
class EffectManager;

class GameScene :public Scene
{
public:
	GameScene(SceneController& controller);
	~GameScene();

	void Init() override;
	void Update(Input& input) override;
	void Draw() override;

	/// <summary>
	/// ステージのサイズを取得する
	/// </summary>
	/// <returns>ステージの大きさ</returns>
	Vector3 const GetStageSize() const;

private:
	/// <summary>
	/// デバッグ用のグリッドを描画する
	/// </summary>
	void DrawGrid();

	/// <summary>
	/// スコアを計算する
	/// </summary>
	/// <param name="deadEnemyNum">倒した敵の数</param>
	/// <returns>計算されたスコア</returns>
	int CalcScore(int deadEnemyNum) const;

private:
	//フレームカウンター
	int m_frameCount;

	//ゲームの制限時間(フレーム数)
	int m_time = 0;

	//ゲーム開始時のカウントダウン
	int m_startCountDown = 0;

	//プレイヤーモデルのコピーハンドル
	int m_playerMHandle = -1;

	//地面のモデルのハンドル
	int m_groundMHandle = -1;

	//敵のモデルのハンドルの配列
	std::vector<int> m_enemyMHandles;

	//skyboxのテクスチャのハンドル
	int m_skyFrontHandle = -1;
	int m_skyBackHandle = -1;
	int m_skyLeftHandle = -1;
	int m_skyRightHandle = -1;
	int m_skyUpHandle = -1;
	int m_skyDownHandle = -1;

	//エフェクシアのエフェクトハンドル
	int m_deathEffectHandle = -1;
	int m_attackFieldEffectHandle = -1;

	//残り時間フォントのハンドル
	int m_timeFontHandle = -1;
	//カウントダウンフォントのハンドル
	int m_countDownFontHandle = -1;

	//敵の数を保持しておく
	int m_enemyCount = 0;

	//プレイヤーの実体
	std::shared_ptr<Player> m_pPlayer;

	//カメラの実体
	std::shared_ptr<Camera> m_pCamera;

	//当たり判定の管理クラスの実体
	std::shared_ptr<CollisionManager> m_pCollManager;

	//敵の生成を行う工場の実体
	std::shared_ptr<EnemyFactory> m_pEnemyFactory;

	//skyboxの実体
	std::shared_ptr<SkyBox> m_pSkyBox;

	//エフェクトマネージャーの実体
	std::shared_ptr<EffectManager> m_pEffectManager;
};