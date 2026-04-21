#pragma once
#include <unordered_map>

class ResourceLoader
{
public:
	//モデルの種類
	enum class ModelID : int
	{
		Player = 0,//プレイヤーのモデル
		Zombie = 1,//ゾンビのモデル
		Ground = 2,//地面のモデル
	};

	//グラフィックの種類
	enum class GraphicID : int
	{
		FrontSky = 0,//skyboxの前のテクスチャ
		BackSky = 1,//skyboxの後ろのテクスチャ
		LeftSky = 2,//skyboxの左のテクスチャ
		RightSky = 3,//skyboxの右のテクスチャ
		UpSky = 4,//skyboxの上のテクスチャ
		DownSky = 5,//skyboxの下のテクスチャ

		TitleLogo = 6,//タイトルロゴの文字グラフィック
		TitleLogoEffect = 7,//タイトルロゴのエフェクトのグラフィック
	};

	//エフェクトの種類
	enum class EffectID : int
	{
		Death = 0,//敵の死亡エフェクト
		AttackField = 1,//攻撃フィールドのエフェクト
	};

	//音の種類
	enum class SoundID :int
	{
		Decision = 0,//決定
		Attack = 1,//攻撃
		PlayerDamage = 2,//プレイヤーがダメージを受けたとき
		EnemyDeath = 3,//敵が死んだとき
		TitleBgm = 4,//タイトルのBGM
		GameBgm = 5,//ゲームシーンのBGM
		ResultBgm = 6,//リザルトシーンのBGM
		Countdown = 7,//カウントダウンの音
		Start = 8,//スタートの音
		AttackStart = 9,//攻撃開始の音
		OnGround = 10,//着地の音
		Thunder = 11,//雷の音
		ScorePop = 12,//スコアポップの音
		RunPlayer = 13,//プレイヤーの足音
	};

public:
	static ResourceLoader& GetInstance();

	//ロード
	void LoadAll();
	//解放
	void ReleaseAll();

	//取得
	int GetModel(ModelID id) const;
	int GetGraphic(GraphicID id) const;
	int GetEffect(EffectID id) const;
	int GetSound(SoundID id) const;

private:
	//=defaultでデフォルトコンストラクタを生成する
	ResourceLoader() = default;
	//デストラクタも同様
	~ResourceLoader() = default;

	//コピーコンストラクタとコピー代入演算子は削除する
	ResourceLoader(const ResourceLoader&) = delete;
	ResourceLoader& operator=(const ResourceLoader&) = delete;

private:
	//IDをいれて直感的にアクセスできるようにするためのマップ
	std::unordered_map<ModelID, int> m_modelHandles;//モデルのハンドルを保存するマップ
	std::unordered_map<GraphicID, int> m_graphicHandles;//グラフィックのハンドルを保存するマップ
	std::unordered_map<EffectID, int> m_effectHandles;//エフェクトのハンドルを保存するマップ
	std::unordered_map<SoundID, int> m_soundHandles;//サウンドのハンドルを保存するマップ
};

