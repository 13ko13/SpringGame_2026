#include "EnemyFactory.h"
#include "../Math/Vector3.h"

namespace
{
	//敵の総数
	constexpr int enemy_num = 10;
}

EnemyFactory::EnemyFactory(std::vector<int> modelHandles) :
	m_handles(modelHandles)
{

}

EnemyFactory::~EnemyFactory()
{
	for (int handle : m_handles)
	{
		MV1DeleteModel(handle);
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

		m_enemies.push_back(std::make_shared<Enemy>(
			handle,//モデルのハンドル
			pos));//生成される位置
		return m_enemies.back();
	}

	default:
		return nullptr;
	}

	return nullptr;
}

void EnemyFactory::Update(const Vector3& playerPos, const Vector3& stageSize)
{
	//常に敵が3体になるようにする
	while (m_enemies.size() < enemy_num)
	{
		//ランダムな位置に敵を生成する
		Vector3 pos = {
			static_cast<float>(rand() % static_cast<int>(stageSize.m_x * 2.0f) - stageSize.m_x),//-1000~1000のランダムなX座標
			0.0f,//Y座標は地面にする
			static_cast<float>(rand() % static_cast<int>(stageSize.m_z * 2.0f) - stageSize.m_z),//-1000~1000のランダムなZ座標
		};

		Create(pos, EnemyType::Enemy1, m_handles);
	}

	for (auto& enemy : m_enemies)
	{
		enemy->Update(playerPos, stageSize);
	}
}

void EnemyFactory::Draw()
{
	for (auto& enemy : m_enemies)
	{
		enemy->Draw();
	}
}