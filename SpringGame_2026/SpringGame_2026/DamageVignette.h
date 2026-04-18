#pragma once
class DamageVignette
{
public:
	void Init(int shaderHandle);
	void Update();
	void Draw();

	/// <summary>
	/// トリガーを起動する
	/// </summary>
	void Trigger();

private:
	//ビネットとは
	//画面の周りが暗くなっているエフェクトのこと
	 
	int m_constBufferHandle = -1;//定数バッファのハンドル
	int m_shaderHandle = -1;//シェーダーのハンドル
	float m_intensity = 0.0f;//ビネットの強さ
	int m_fadeTimer = 0;//フェードのタイマー

	//定数バッファにパラメータをセットする
	struct ShaderParameter
	{
		float vignetteColor[4];//ビネットの色
		float vignetteIntensity;//ビネットの強さ
		float vignettePower;//ビネットの強さの累乗
		float padding[2];//パディング(定数バッファのサイズを16の倍数にするため)
	};

private:
	enum class VignetteColor
	{
		Red,
		Green,
		Blue,
		Alpha
	};
};

