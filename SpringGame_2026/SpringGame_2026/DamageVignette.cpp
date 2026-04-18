#include "DamageVignette.h"
#include "Main/Application.h"
#include <DxLib.h>

namespace
{
	//何フレームかけてビネットの強さを0にするか
	constexpr float fade_frame = 60;

	//ビネットの色の要素数(RGBA)
	constexpr int color_num = 4;

	//ビネットのカーブ
	constexpr float curve = 2.5f;
}

void DamageVignette::Init(int shaderHandle)
{
	m_shaderHandle = shaderHandle;//シェーダーのハンドルを保存する
	
	//定数バッファを作成する
	m_constBufferHandle = CreateShaderConstantBuffer(sizeof(ShaderParameter));//定数バッファのサイズはShaderParameter構造体のサイズにする
}

void DamageVignette::Update()
{
	if (m_fadeTimer > 0)
	{
		m_fadeTimer--;
		//徐々にビネットの強さを弱くする
		m_intensity = static_cast<float>(m_fadeTimer) / fade_frame;//60フレームかけてビネットの強さを0にする
	}
	else
	{
		m_intensity = 0.0f;//ビネットの強さを0にする
	}
}

void DamageVignette::Draw()
{
	if (m_intensity <= 0.0f) return;//ビネットの強さが0以下のときは描画しない

	//シェーダーを有効化
	SetUsePixelShader(m_shaderHandle);

	ShaderParameter param;//シェーダーに渡すパラメータを作成する
	param.vignetteColor[static_cast<int>(VignetteColor::Red)] = 1.0f;//ビネットの色のR要素
	param.vignetteColor[static_cast<int>(VignetteColor::Green)] = 0.0f;//ビネットの色のG要素
	param.vignetteColor[static_cast<int>(VignetteColor::Blue)] = 0.0f;//ビネットの色のB要素
	param.vignetteColor[static_cast<int>(VignetteColor::Alpha)] = m_intensity;//ビネットの色のA要素(透明度)
	param.vignetteIntensity = m_intensity;
	param.vignettePower = curve;

	//定数バッファのアドレスを取得する
	void* pBuffer = GetBufferShaderConstantBuffer(m_constBufferHandle);

	//アドレスを使ってデータをコピーする
	memcpy(pBuffer, &param, sizeof(param));//メモリ領域をコピーする関数。第一引数はコピー先、第二引数はコピー元、第三引数はコピーするバイト数
	//GPUに更新された定数バッファの内容を知らせる
	UpdateShaderConstantBuffer(m_constBufferHandle);
	//シェーダーに定数バッファをセットする
	SetShaderConstantBuffer(m_constBufferHandle, DX_SHADERTYPE_PIXEL, 0);//第3引数は定数バッファの番号。シェーダー側で定数バッファを宣言するときのregister(b0)のb0の部分に対応する

	//画面全体を描画する(シェーダーが適用される)
	auto& wSize = Application::GetInstance().GetWindowSize();
	DrawBox(0, 0, wSize.w, wSize.h, 0xffffff, TRUE);

	//シェーダーを無効化する
	SetUsePixelShader(-1);
}

void DamageVignette::Trigger()
{
	m_fadeTimer = fade_frame;//フェードのタイマーをリセットする
	m_intensity = 1.0f;//ビネットの強さを最大にする
}
