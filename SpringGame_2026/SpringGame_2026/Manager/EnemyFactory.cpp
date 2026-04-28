#include "EnemyFactory.h"
#include "../Math/Vector3.h"

namespace
{
	//敵を最初に生成するときの数
	constexpr int enemy_num = 30;

	//ステージの辺の数
	constexpr int stage_side_num = 4;

	//プレイヤーからどのくらい離れた位置に生成するかの最小距離
	constexpr float create_enemy_min_distance = 500.0f;
	//プレイヤーからどのくらい離れた位置に生成するかの最大距離
	constexpr float create_enemy_max_distance = 1400.0f;
}

EnemyFactory::EnemyFactory(std::vector<int> modelHandles, std::shared_ptr<EffectManager> pEffectManager) :
	m_handles(modelHandles),
	m_pEffectManager(pEffectManager)
{

}

EnemyFactory::~EnemyFactory()
{

}

void EnemyFactory::Init(const Vector3& stageSize, const Vector3& playerPos)
{
	//敵がenemy_num体になるようにする
	while (m_pEnemies.size() < enemy_num)
	{
		//プレイヤーから一定距離以上離れた位置にランダムに敵を生成する
		Vector3 pos;
		
		//プレイヤーからの最小距離以上、最大距離の範囲でランダムな距離
		float distance = create_enemy_min_distance + static_cast<float>(rand()) / RAND_MAX * (create_enemy_max_distance - create_enemy_min_distance);
		//ランダムな角度を作る
		float angle = static_cast<float>(rand()) / RAND_MAX * DX_TWO_PI_F;
		//極座標(距離と角度)を直交座標(x,z)に変換する
		pos.m_x = playerPos.m_x + distance * cosf(angle);
		pos.m_z = playerPos.m_z + distance * sinf(angle);

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