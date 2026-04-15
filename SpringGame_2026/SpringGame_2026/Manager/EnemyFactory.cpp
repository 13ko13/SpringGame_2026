#include "EnemyFactory.h"
#include "../Math/Vector3.h"

namespace
{
	//敵を最初に生成するときの数
	constexpr int enemy_num = 32;
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
	//常に敵がenemy_num体になるようにする
	while (m_pEnemies.size() < enemy_num)
	{
		//ランダムな位置に敵を生成する
		Vector3 pos = {
			static_cast<float>(rand() % static_cast<int>(stageSize.m_x * 2.0f) - stageSize.m_x),//-1000~1000のランダムなX座標
			0.0f,//Y座標は地面にする
			static_cast<float>(rand() % static_cast<int>(stageSize.m_z * 2.0f) - stageSize.m_z),//-1000~1000のランダムなZ座標
		};

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

void EnemyFactory::Update(const Vector3& playerPos, const Vector3& stageSize)
{
	for (auto& enemy : m_pEnemies)
	{
		enemy->Update(playerPos, stageSize);

		//敵一体一体の押し戻しの処理
		for (auto& other : m_pEnemies)
		{
			//自分自身との押し戻しは行わない
			if (enemy == other) continue;
			//2体の距離ベクトルを求める
			Vector3 toOther = other->GetPos() - enemy->GetPos();
			//そのベクトルの長さを求める
			float distance = toOther.Length();

			//自分の球の半径
			const float myRadius = enemy->GetSphere().GetRadius();
			//他の敵の球の半径
			const float otherRadius = other->GetSphere().GetRadius();

			//距離がそれぞれの円の半径の和より小さいとき、押し戻しの処理を行う
			if(distance < myRadius + otherRadius)
			{
				//押し戻す方向と強さを表すベクトルを求める
				Vector3 pushVector = toOther.Normalized() * (myRadius + otherRadius - distance);
				//押し戻す処理を行う
				enemy->OnPushBack(-pushVector);
				other->OnPushBack(pushVector);
			}
		}
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