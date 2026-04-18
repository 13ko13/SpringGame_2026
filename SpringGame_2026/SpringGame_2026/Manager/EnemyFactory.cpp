#include "EnemyFactory.h"
#include "../Math/Vector3.h"

namespace
{
	//敵を最初に生成するときの数
	constexpr int enemy_num = 34;

	//ステージの辺の数
	constexpr int stage_side_num = 4;
}

EnemyFactory::EnemyFactory(std::vector<int> modelHandles, std::shared_ptr<EffectManager> pEffectManager) :
	m_handles(modelHandles),
	m_pEffectManager(pEffectManager)
{

}

EnemyFactory::~EnemyFactory()
{

}

void EnemyFactory::Init(const Vector3& stageSize)
{
	//敵がenemy_num体になるようにする
	while (m_pEnemies.size() < enemy_num)
	{
		//敵をステージ外周のランダムな位置に生成する
		//どの辺から生成するをかを決める
		int randSide = rand() % stage_side_num;
		
		Vector3 pos;

		enum class Side
		{
			Left = 0,
			Right,
			Up,
			Down,
		};

		switch (static_cast<Side>(randSide))
		{
		case Side::Left:
			pos = { -stageSize.m_x,//左側の辺のx座標
					0.0f,
					static_cast<float>(rand() % static_cast<int>(stageSize.m_z * 2.0f) - stageSize.m_z) };//辺のz座標の範囲は-stageSize.m_zからstageSize.m_zまで
			break;
		case Side::Right:
			pos = { stageSize.m_x,//右側の辺のx座標
					0.0f,
					static_cast<float>(rand() % static_cast<int>(stageSize.m_z * 2.0f) - stageSize.m_z) };//辺のz座標の範囲は-stageSize.m_zからstageSize.m_zまで
			break;
		case Side::Up:
			pos = { static_cast<float>(rand() % static_cast<int>(stageSize.m_x * 2.0f) - stageSize.m_x),//辺のx座標の範囲は-stageSize.m_xからstageSize.m_xまで
					0.0f,
					-stageSize.m_z };//上側の辺のz座標
			break;
		case Side::Down:
			pos = { static_cast<float>(rand() % static_cast<int>(stageSize.m_x * 2.0f) - stageSize.m_x),//辺のx座標の範囲は-stageSize.m_xからstageSize.m_xまで
					0.0f,
					stageSize.m_z };//下側の辺のz座標
			break;
		}

		Create(pos, EnemyType::Enemy1, m_handles);
	}
}

std::shared_ptr<Enemy> EnemyFactory::Create(const Vector3& pos, EnemyType enemyType, std::vector<int> enemyHandles)
{
	switch (enemyType)
	{
	case EnemyType::Enemy1:
	{
		//モデルをコピーして、コピーしたモデルのハンドルを渡す
		int handle = MV1DuplicateModel(enemyHandles[static_cast<int>(enemyType)]);

		m_pEnemies.push_back(std::make_shared<Enemy>(
			handle,//モデルのハンドル
			pos,//生成される位置
			m_pEffectManager));//エフェクトマネージャー
		return m_pEnemies.back();
	}

	default:
		return nullptr;
	}

	return nullptr;
}

void EnemyFactory::Update(const Vector3& playerPos, const Vector3& stageSize, bool isCanMove)
{
	for (auto& enemy : m_pEnemies)
	{
		enemy->Update(playerPos, stageSize, isCanMove);
	}

	//敵が死んでいるときはリストから削除する
	auto it = std::remove_if(m_pEnemies.begin(), m_pEnemies.end(),
		[](const std::shared_ptr<Enemy>& enemy)
		{
			return enemy->IsDead();
		});

	m_pEnemies.erase(it, m_pEnemies.end());
}

void EnemyFactory::Draw()
{
	for (auto& enemy : m_pEnemies)
	{
		enemy->Draw();
	}
}