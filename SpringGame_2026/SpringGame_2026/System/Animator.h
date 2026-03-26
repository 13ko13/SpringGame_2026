#pragma once

class Animator
{
public:
	Animator(int modelHandle);
	~Animator();

	/// <summary>
	/// アニメーションを再生(変更)
	/// </summary>
	/// <param name="animIndex">再生させるアニメーション番号</param>
	/// <param name="blendTime">何フレームでブレンドするか</param>
	void Play(int animIndex, float blendTime);

	/// <summary>
	/// アニメーションを更新
	/// </summary>
	/// <param name="durationTime">ブレンドの間隔</param>
	void Update(float durationTime);

private:
	//モデルのハンドル
	int m_modelHandle = -1;

	//現在再生中のアニメーションの番号
	int m_currentAnim = -1;
	//次に再生するアニメーションの番号
	int m_nextAnim = -1;

	//現在再生中のアニメーション時間
	float m_currentTime = 0.0f;
	//次に再生するアニメーションの時間
	float m_nextTime = 0.0f;

	//ブレンドするときのタイマー
	float m_blendTimer = 0.0f;
	//ブレンドするときの間隔
	float m_blendDuration = 0.0f;

	//現在ブレンド中か
	bool m_isBlending = false;
};

