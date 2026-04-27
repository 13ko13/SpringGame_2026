#pragma once
#include <string>

class TextUI
{
public:
	/// <summary>
	/// フォント設定を使用してTextUIオブジェクトを構築
	/// </summary>
	/// <param name="fontHandle">フォントリソースのハンドル</param>
	/// <param name="fontSize">フォントのサイズ</param>
	TextUI(int fontHandle, int fontSize);
	~TextUI();

	/// <summary>
	/// 画面真ん中に文字を描画する
	/// </summary>
	/// <param name="text">描画するテキスト</param>
	/// <param name="color">描画するテキストの色</param>
	/// <param name="scaleX">X方向の拡大率</param>
	/// <param name="scaleY">Y方向の拡大率</param>
	/// <param name="edgeColor">文字の縁の色</param>
	void DrawCenter(const std::string& text,
					unsigned int color,
					double scaleX = 1.0,
					double scaleY = 1.0,
					unsigned int edgeColor = 0xffffff) const;

	/// <summary>
	/// 画面の割合位置に文字を描画する(0.0～1.0で指定)
	/// </summary>
	/// <param name="text">描画するテキスト</param>
	/// <param name="xRate">画面の幅に対するX座標の割合</param>
	/// <param name="yRate">画面の高さに対するY座標の割合</param>
	/// <param name="color">描画するテキストの色</param>
	/// <param name="scaleX">X方向の拡大率</param>
	/// <param name="scaleY">Y方向の拡大率</param>
	/// <param name="edgeColor">文字の縁の色</param>
	void DrawAtRate(const std::string& text, float xRate, float yRate, unsigned int color,
		double scaleX = 1.0, double scaleY = 1.0, unsigned int edgeColor = 0xffffff) const;

private:
	//フォントハンドル
	int handle = -1;
	//フォントサイズ
	int size = 0;
	//フォントスタイル
	int style = 0;
};

