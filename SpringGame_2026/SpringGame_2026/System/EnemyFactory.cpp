#include "EnemyFactory.h"
#include "../Math/Vector3.h"
#include "../GameObjects/Enemy.h"

EnemyFactory::EnemyFactory(std::vector<int> modelHandles) :
	m_handles(modelHandles)
{

}

EnemyFactory::~EnemyFactory()
{
}

std::shared_ptr<Enemy> EnemyFactory::Create(const Vector3& pos, EnemyType enemyType)
{
	switch (enemyType)
	{
	case EnemyType::Enemy1:
	{
		m_enemies.push_back(std::make_shared<Enemy>(
			m_handles[static_cast<int>(EnemyType::Enemy1)],//モデルのハンドル
			pos));//生成される位置
		return m_enemies.back();
	}

	default:
		return nullptr;
	}

	return nullptr;
}

void EnemyFactory::Update()
{
	for (auto& enemy : m_enemies)
	{
		enemy->Update();
	}
}

void EnemyFactory::Draw()
{
	for (auto& enemy : m_enemies)
	{
		enemy->Draw();
	}
}
