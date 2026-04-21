#include "ResourceLoader.h"
#include <DxLib.h>
#include <cassert>
#include <EffekseerForDXLib.h>

namespace
{
	//エフェクトの拡大率
	constexpr float attack_ef_scale = 10.0f;
}

ResourceLoader& ResourceLoader::GetInstance()
{
	//staticでインスタンスを宣言してそれを返す
	static ResourceLoader instance;
	return instance;
}

void ResourceLoader::LoadAll()
{
	//----モデル-------
	//モデルを読み込んでハンドルを保存する
	int handle = -1;

	handle = MV1LoadModel("Data/Mv1/Player.mv1");//プレイヤーのモデル
	assert(handle != -1);
	m_modelHandles[ModelID::Player] = handle;

	handle = MV1LoadModel("Data/Mv1/Enemy.mv1");//敵のモデル
	assert(handle != -1);
	m_modelHandles[ModelID::Zombie] = handle;

	handle = MV1LoadModel("Data/Mv1/Ground.mv1");//地面のモデル
	assert(handle != -1);
	m_modelHandles[ModelID::Ground] = handle;

	//----グラフィック------
	//skyboxのテクスチャのロード
	handle = LoadGraph("Data/SkyBox/Cloudy_SkyBox_3/skybox_front.png");//skyboxの前のテクスチャ
	assert(handle != -1);
	m_graphicHandles[GraphicID::FrontSky] = handle;

	handle = LoadGraph("Data/SkyBox/Cloudy_SkyBox_3/skybox_back.png");//skyboxの後ろのテクスチャ
	assert(handle != -1);
	m_graphicHandles[GraphicID::BackSky] = handle;

	handle = LoadGraph("Data/SkyBox/Cloudy_SkyBox_3/skybox_left.png");//skyboxの左のテクスチャ
	assert(handle != -1);
	m_graphicHandles[GraphicID::LeftSky] = handle;

	handle = LoadGraph("Data/SkyBox/Cloudy_SkyBox_3/skybox_right.png");//skyboxの右のテクスチャ
	assert(handle != -1);
	m_graphicHandles[GraphicID::RightSky] = handle;

	handle = LoadGraph("Data/SkyBox/Cloudy_SkyBox_3/skybox_up.png");//skyboxの上のテクスチャ
	assert(handle != -1);
	m_graphicHandles[GraphicID::UpSky] = handle;

	handle = LoadGraph("Data/SkyBox/Cloudy_SkyBox_3/skybox_down.png");//skyboxの下のテクスチャ
	assert(handle != -1);
	m_graphicHandles[GraphicID::DownSky] = handle;

	handle = LoadGraph("Data/Graphic/Title_Logo_3.png");//タイトルロゴのグラフィック
	assert(handle != -1);
	m_graphicHandles[GraphicID::TitleLogo] = handle;

	handle = LoadGraph("Data/Graphic/Title_Logo_Effect.png");//タイトルロゴのエフェクトのグラフィック
	assert(handle != -1);
	m_graphicHandles[GraphicID::TitleLogoEffect] = handle;

	//----エフェクト------
	handle = LoadEffekseerEffect("Data/Effect/Death.efk");//敵の死亡エフェクト
	assert(handle != -1);
	m_effectHandles[EffectID::Death] = handle;

	handle = LoadEffekseerEffect("Data/Effect/AttackField.efk", attack_ef_scale);//攻撃フィールドのエフェクト
	assert(handle != -1);
	m_effectHandles[EffectID::AttackField] = handle;

	//----サウンド------
	handle = LoadSoundMem("Data/Sound/decision.mp3");//決定音
	assert(handle != -1);
	m_soundHandles[SoundID::Decision] = handle;

	handle = LoadSoundMem("Data/Sound//dead_enemy.mp3");//敵が死んだときの音
	assert(handle != -1);
	m_soundHandles[SoundID::EnemyDeath] = handle;

	handle = LoadSoundMem("Data/Sound/title_bgm.mp3");//タイトルのBGM
	assert(handle != -1);
	m_soundHandles[SoundID::TitleBgm] = handle;

	handle = LoadSoundMem("Data/Sound/game_bgm.mp3");//ゲームシーンのBGM
	assert(handle != -1);
	m_soundHandles[SoundID::GameBgm] = handle;

	handle = LoadSoundMem("Data/Sound/result_bgm.mp3");//リザルトシーンのBGM
	assert(handle != -1);
	m_soundHandles[SoundID::ResultBgm] = handle;

	handle = LoadSoundMem("Data/Sound/countdown.mp3");//カウントダウンの音
	assert(handle != -1);
	m_soundHandles[SoundID::Countdown] = handle;

	handle = LoadSoundMem("Data/Sound/start_sound.mp3");//スタートの音
	assert(handle != -1);
	m_soundHandles[SoundID::Start] = handle;

	handle = LoadSoundMem("Data/Sound/attack_start.mp3");//攻撃開始の音
	assert(handle != -1);
	m_soundHandles[SoundID::AttackStart] = handle;

	handle = LoadSoundMem("Data/Sound/on_ground.mp3");//着地の音
	assert(handle != -1);
	m_soundHandles[SoundID::OnGround] = handle;

	handle = LoadSoundMem("Data/Sound/hit_player.mp3");//プレイヤーが攻撃を受けたときの音
	assert(handle != -1);
	m_soundHandles[SoundID::PlayerDamage] = handle;

	handle = LoadSoundMem("Data/Sound/thunder.mp3");//雷の音
	assert(handle != -1);
	m_soundHandles[SoundID::Thunder] = handle;

	handle = LoadSoundMem("Data/Sound/score_pop.mp3");//スコアポップの音
	assert(handle != -1);
	m_soundHandles[SoundID::ScorePop] = handle;

	handle = LoadSoundMem("Data/Sound/run.mp3");//プレイヤーの足音
	assert(handle != -1);
	m_soundHandles[SoundID::RunPlayer] = handle;
}

void ResourceLoader::ReleaseAll()
{
	//すべてのリソースを解放する
	//モデル
	for (auto& modelH : m_modelHandles)
	{
		MV1DeleteModel(modelH.second);
	}
	//グラフィック
	for (auto& graphH : m_graphicHandles)
	{
		DeleteGraph(graphH.second);
	}
	//エフェクトはEffekseerのEnd関数でまとめて解放されるため、個別に解放する必要はない
}

int ResourceLoader::GetModel(ModelID id) const
{
	//IDをもとにハンドルを返す
	auto it = m_modelHandles.find(id);
	
	//it != m_modelHandles.end()は、idに対応するハンドルが見つかったかどうかをチェックしている
	//end()は、マップの最後を指すイテレータで、find()が見つからなかったときに返される
	if (it != m_modelHandles.end())
	{
		return it->second;
	}
	else
	{
		assert(false && "モデルIDが見つかりません");
		return -1;
	}
}

int ResourceLoader::GetGraphic(GraphicID id) const
{
	//IDをもとにハンドルを返す
	auto it = m_graphicHandles.find(id);

	if(it != m_graphicHandles.end())
	{
		return it->second;
	}
	else
	{
		assert(false && "グラフィックIDが見つかりません");
		return -1;
	}
}

int ResourceLoader::GetEffect(EffectID id) const
{
	auto it = m_effectHandles.find(id);

	if(it != m_effectHandles.end())
	{
		return it->second;
	}
	else
	{
		assert(false && "エフェクトIDが見つかりません");
		return -1;
	}
}

int ResourceLoader::GetSound(SoundID id) const
{
	auto it = m_soundHandles.find(id);

	if (it != m_soundHandles.end())
	{
		return it->second;
	}
	else
	{
		assert(false && "サウンドIDが見つかりません");
		return -1;
	}
}
