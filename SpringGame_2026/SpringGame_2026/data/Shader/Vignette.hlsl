//テクスチャとサンプラー(元の画面)
//DirectX 11以降では、テクスチャとサンプラーは別々に定義されることが一般的
Texture2D DiffuseTexture : register(t0);
SamplerState DiffuseSampler : register(s0);

//定数バッファ(C++から渡されるパラメータ）
cbuffer VignetteParameters : register(b0)
{
	float4 VignetteColor; //ビネットの色
    float VignetteIntensity; //ビネットの強さ
	float VignettePower; //ビネットのカーブ
    float2 Padding; //パディング（16バイトアラインメントのため）
};

//ピクセルシェーダーのエントリーポイント
//TexCoordは今処理しているピクセルのテクスチャ座標:例えば(0,0)が左下、(1,1)が右上
//返り値はこのピクセルの最終的な色
float4 main(float2 texCoord : TEXCOORD0) : SV_TARGET
{
	//元の画面の色を取得する
    float4 color = DiffuseTexture.Sample(DiffuseSampler, texCoord);
    
    //画面中心からの距離を計算する
    float2 center = float2(0.5, 0.5); //画面の中心
    float dist= length(texCoord - center); //中心からの距離
    
    //ビネット効果を計算する(中心が0になり、端が1になるように)
    //距離を累乗してカーブを作る
    float vignette = pow(dist, VignettePower) * VignetteIntensity; //距離に基づいてビネットの強さを計算
    //0～1の範囲にクランプする
    vignette = saturate(vignette);
    
    //元の色とビネットの色を混ぜる
    color.rgb = lerp(color.rgb, VignetteColor.rgb, vignette * VignetteColor.a); //ビネットの強さに基づいて色を混ぜる
    return color; //最終的な色を返す
}