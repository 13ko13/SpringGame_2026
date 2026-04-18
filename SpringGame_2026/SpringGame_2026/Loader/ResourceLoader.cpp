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

	//----シェーダー------
	handle = LoadPixelShader("data/Shader/Vignette.pso");//ダメージビネットのシェーダー
	assert(handle != -1);
	m_shaderHandles[ShaderID::DamageVignette] = handle;
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

	//シェーダー
	for (auto& shaderH : m_shaderHandles)
	{
		DeleteShader(shaderH.second);
	}
}

int ResourceLoader::GetModel(ModelID id) const
{
	//IDをもとにハンドルを返す
	auto it = m_modelHandles.find(id);
	
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

int ResourceLoader::GetShader(ShaderID id) const
{
	//シェーダーのハンドルをIDをもとに返す
	auto it = m_shaderHandles.find(id);

	//見つかったときはハンドルを返す
	if (it != m_shaderHandles.end())
	{
		return it->second;
	}
	else
	{
		assert(false && "シェーダーIDが見つかりません");
		return -1;
	}
}
