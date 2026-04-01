#include "EnemyFactory.h"
#include "../Math/Vector3.h"

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

std::shared_ptr<Enemy> EnemyFactory::Create(const Vector3& pos, EnemyType enemyType,std::vector<int> enemyHandles)
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

void EnemyFactory::Update(const Vector3& playerPos)
{
	//常に敵が3体になるようにする
	while (m_enemies.size() < 3)
	{
		//ランダムな位置に敵を生成する
		Vector3 pos = {
			static_cast<float>(rand() % 2000 - 1000),//-1000~1000のランダムなX座標
			0.0f,//Y座標は地面にする
			static_cast<float>(rand() % 2000 - 1000),//-1000~1000のランダムなZ座標
		};

		Create(pos, EnemyType::Enemy1, m_handles);
	}

	for (auto& enemy : m_enemies)
	{
		enemy->Update(playerPos);
	}
}

void EnemyFactory::Draw()
{
	for (auto& enemy : m_enemies)
	{
		enemy->Draw();
	}
}