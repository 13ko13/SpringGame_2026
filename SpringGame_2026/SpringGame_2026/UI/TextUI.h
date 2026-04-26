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

	/// <summary>
	/// 文字を順番に表示する演出を開始する
	/// </summary>
	void StartStamp(
		const std::string& text,//表示するテキスト
		float interval,//文字が表示される速度(秒)
		float stampFrame,//スタンプにかけるフレーム数
		float startScale//スタンプの開始時の拡大率
	);

private:
	//フォントハンドル
	int handle = -1;
	//フォントサイズ
	int size = 0;
	//フォントスタイル
	int style = 0;

	//文字を順番に出すときの経過フレーム数
	int m_frame = 0;

	//タイピング中かどうか
	bool m_isTyping = false;

	//現在何文字目まで表示するか
	int m_currentCharIndex = 0;

	//全体の文字列を保持する
	std::string m_fullText;

	//文字が表示される速度
	float m_interval = 1.0f;

	//スタンプにかけるフレーム数
	float m_stampFrame = 0.0f;

	//スタンプの開始時の拡大率
	float m_startScale = 1.0f;

private:
	struct CharState
	{
		int startFrame; // 文字が表示され始めるフレーム
		float currentScale; // 現在の拡大率
		bool isCompleted; // 文字の表示が完了したかどうか
	};

	std::vector<CharState> m_charStates; // 各文字の状態を保持するベクター

private:
	void UpdateStamp();

	/// <summary>
	/// UTF-8文字列の文字数を数える関数
	/// </summary>
	/// <param name="str">文字数を数える対象のUTF-8文字列</param>
	/// <returns>文字数</returns>
	int CountUTF8Chars(const std::string& str) const;
};

