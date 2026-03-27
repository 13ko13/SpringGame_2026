#include "Animator.h"
#include <DxLib.h>

Animator::Animator(int modelHandle):
	m_modelHandle(modelHandle)
{

}

Animator::~Animator()
{
}

void Animator::Play(int animIndex)
{
	//同じアニメーション中ならアタッチしない
	if (m_animIdx == animIndex) return;

	//再生中のアニメーション番号を更新
	m_animIdx = animIndex;

	//もしすでに前のアニメーションが残っていたら強制終了してデタッチする
	if (m_prevAttachIdx != -1)
	{
		MV1DetachAnim(m_modelHandle, m_prevAttachIdx);
	}

	//現在再生中のアニメーションを一つ前のアニメーションとする
	m_prevAttachIdx = m_currentAttachIdx;//現在のアニメーションを保存する
	m_prevTime = m_currentTime;//現在のアニメーションのカウントを保存する

	//アニメーションをアタッチ
	m_currentAttachIdx= MV1AttachAnim(m_modelHandle, animIndex);
	m_currentTime = 0.0f;

	//ブレンド中のタイマーをリセット
	m_blendTimer = 0.0f;

	//前のがあればブレンド開始、なければ（初回再生なら）即100%
	if (m_prevAttachIdx != -1) {
		m_isBlending = true;
	}
	else {
		m_isBlending = false;
		MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAttachIdx, 1.0f);
	}
}

void Animator::Update(float blendTime)
{
	//アニメーションなし状態なら処理をスキップ
	if (m_currentAttachIdx < 0 ) return;

	m_currentTime += 0.5f;//アニメーションの時間を更新
	m_prevTime += 0.5f;

	float totalTime = MV1GetAttachAnimTotalTime(m_modelHandle, m_currentAttachIdx);
	//アニメーションのカウントが総カウントを越えたら
	//アニメーションのカウントをリセットする
	if (m_currentTime >= totalTime)
	{
		m_currentTime -= totalTime;
	}

	//アニメーションのカウントをアニメーションに反映させる
	MV1SetAttachAnimTime(m_modelHandle, m_currentAttachIdx, m_currentTime);

	//ブレンドフラグが立っていたら
	if (m_isBlending && m_prevAttachIdx != -1)
	{
		m_blendTimer++;

		//アニメーションのブレンド率を変更して滑らかな切り替えを行う
		if (m_prevAttachIdx == -1)
		{
			//アニメーションが1つしか設定されていない場合
			//ブレンドは行わない
			MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAttachIdx, 1.0f);//アニメーションのブレンド率を1にする
		}
		else
		{
			//現在のアニメーションのブレンド率を計算する
			float blendRate = m_blendTimer / blendTime;//ブレンドにかける時間をもとに、ブレンド率を計算する
			//ブレンド率が1を越えたら
			//ブレンド率を1ににする
			if (blendRate > 1.0f)
			{
				blendRate = 1.0f;
				//ブレンド中フラグを降ろす
				m_isBlending = false;

				//前のアニメーションをデタッチする
				MV1DetachAnim(m_modelHandle, m_prevAttachIdx);
			}

			MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAttachIdx, blendRate);
			MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAttachIdx, 1.0 - blendRate);
		}

		//アニメーションのループ再生
		//アニメーションの総カウントを取得する
		float totalTime = MV1GetAttachAnimTotalTime(m_modelHandle, m_currentAttachIdx);
		
		//アニメーションのカウントが総カウントを越えたら
		//アニメーションのカウントをリセットする
		if (m_currentTime >= totalTime)
		{
			m_currentTime -= totalTime;
		}
		//一つ前のアニメーションも一応カウントを進める
		if (m_prevAttachIdx != -1)
		{
			//アニメーションのループ再生
		//アニメーションの総カウントを取得する
			float totalTime = MV1GetAttachAnimTotalTime(m_modelHandle, m_prevAttachIdx);

			//アニメーションのカウントが総カウントを越えたら
			//アニメーションのカウントをリセットする
			if (m_prevTime >= totalTime)
			{
				m_prevTime -= totalTime;
			}

			//前のアニメーションのカウントをアニメーションに反映させる
			MV1SetAttachAnimTime(m_modelHandle, m_prevAttachIdx, m_prevTime);
		}
	}
}
