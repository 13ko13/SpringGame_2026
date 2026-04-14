#include "ResourceLoader.h"
#include <DxLib.h>
#include <cassert>
#include <EffekseerForDXLib.h>

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
	handle = LoadGraph("Data/SkyBox/skybox_front.png");//skyboxの前のテクスチャ
	assert(handle != -1);
	m_graphicHandles[GraphicID::FrontSky] = handle;

	handle = LoadGraph("Data/SkyBox/skybox_back.png");//skyboxの後ろのテクスチャ
	assert(handle != -1);
	m_graphicHandles[GraphicID::BackSky] = handle;

	handle = LoadGraph("Data/SkyBox/skybox_left.png");//skyboxの左のテクスチャ
	assert(handle != -1);
	m_graphicHandles[GraphicID::LeftSky] = handle;

	handle = LoadGraph("Data/SkyBox/skybox_right.png");//skyboxの右のテクスチャ
	assert(handle != -1);
	m_graphicHandles[GraphicID::RightSky] = handle;

	handle = LoadGraph("Data/SkyBox/skybox_up.png");//skyboxの上のテクスチャ
	assert(handle != -1);
	m_graphicHandles[GraphicID::UpSky] = handle;

	handle = LoadGraph("Data/SkyBox/skybox_down.png");//skyboxの下のテクスチャ
	assert(handle != -1);
	m_graphicHandles[GraphicID::DownSky] = handle;

	//----エフェクト------
	handle = LoadEffekseerEffect("Data/Effect/Death.efk");//敵の死亡エフェクト
	assert(handle != -1);
	m_effectHandles[EffectID::Death] = handle;

	handle = LoadEffekseerEffect("Data/Effect/AttackField.efk");//攻撃フィールドのエフェクト
	assert(handle != -1);
	m_effectHandles[EffectID::AttackField] = handle;
}

void ResourceLoader::ReleaseAll()
{
}

int ResourceLoader::GetModel(ModelID id) const
{
	return 0;
}

int ResourceLoader::GetGraphic(GraphicID id) const
{
	return 0;
}

int ResourceLoader::GetEffect(EffectID id) const
{
	return 0;
}
