#include "Animator.h"
#include <DxLib.h>

Animator::Animator(int modelHandle):
	m_modelHandle(modelHandle)
{

}

Animator::~Animator()
{
}

void Animator::Play(int animIndex, float blendTime)
{
	//同じアニメーションを再生中なら何もしない
	if (m_currentAnim == animIndex) return;

	if(m_currentAnim)
}

void Animator::Update(float durationTime)
{
	//アニメーションなし状態なら処理をスキップ
	if (m_currentAnim < 0) return;

	//アニメーション再生時間を更新する
	MV1SetAttachAnimTime(m_modelHandle, m_currentAnim, m_currentTime);
	m_currentTime += durationTime;//durationTimeずつ増えていく(durationTimeが高いほどアニメーション速度が速くなる)

	//ブレンドフラグが立っていたら
	if (m_isBlending)
	{
		//アニメーションの再生時間を更新
		MV1SetAttachAnimTime(m_modelHandle, m_nextAnim, m_nextTime);
		m_nextTime += durationTime;

		//ブレンドタイマーを更新(ブレンド開始からの経過時間)
		m_blendTimer += durationTime;
		//ブレンドの割合を計算
		float blendRate = m_blendTimer / m_blendDuration;

		//現在のアニメーションの割合を計算
		MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnim, 1.0f - blendRate);
		//次のアニメーションの割合はそのまま使う
		MV1SetAttachAnimBlendRate(m_modelHandle, m_nextAnim, blendRate);

		//ブレンドが完了していたら
		if (blendRate >= 1.0f)
		{
			//現在のアニメーションに次のアニメーションを設定する
			m_currentAnim = m_nextAnim;
			//現在のアニメーション時間に次のアニメーション時間を設定する
			m_currentTime = m_nextTime;
			
			//ブレンド中フラグを降ろす
			m_isBlending = false;
		}
	}
}
