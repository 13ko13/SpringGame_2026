#include <DxLib.h>

#include "TextUI.h"
#include "../System/ToKanji.h"
#include "../Main/Application.h"
#include "../Math/Size.h"
#include "../Math/Vector2.h"

TextUI::TextUI(int fontHandle, int fontSize) :
	handle(fontHandle),
	size(fontSize)
{
}

TextUI::~TextUI()
{
}

void TextUI::DrawCenter(
	const std::string& text,
	unsigned int color, double scaleX,
	double scaleY, unsigned int edgeColor) const
{
	//画面のサイズを取得する
	auto& wSize = Application::GetInstance().GetWindowSize();

	//文字列のサイズを計算
	Size strSize = {
		static_cast<int>(GetDrawStringWidthToHandle(text.c_str(),
			static_cast<int>(strlen(text.c_str())), handle) * scaleX),
		static_cast<int>(size * scaleY)
	};

	//文字列の描画位置を計算(画面の中心に配置するため)
	Vector2 drawPos;
	drawPos.m_x = wSize.m_width / 2.0f - strSize.m_width / 2.0f;
	drawPos.m_y = wSize.m_height / 2.0f - strSize.m_height / 2.0f;
	DrawExtendStringToHandle(
		static_cast<int>(drawPos.m_x),
		static_cast<int>(drawPos.m_y),
		scaleX, scaleY, text.c_str(),
		color, handle, edgeColor);
}

void TextUI::DrawAtRate(
	const std::string& text,
	float xRate, float yRate,
	unsigned int color, double scaleX,
	double scaleY, unsigned int edgeColor) const
{
	//画面のサイズを取得する
	auto& wSize = Application::GetInstance().GetWindowSize();
	//文字列のサイズを計算
	Size strSize = {
		static_cast<int>(GetDrawStringWidthToHandle(text.c_str(),static_cast<int>(strlen(text.c_str())), handle) * scaleX),
		static_cast<int>(size * scaleY)
	};
	//文字列の描画位置を計算(画面の割合位置に配置するため)
	Vector2 drawPos;
	drawPos.m_x = wSize.m_width * xRate - strSize.m_width / 2.0f;
	drawPos.m_y = wSize.m_height * yRate - strSize.m_height / 2.0f;

	//描画
	DrawExtendStringToHandle(
		static_cast<int>(drawPos.m_x),
		static_cast<int>(drawPos.m_y),
		scaleX, scaleY, text.c_str(),
		color, handle, edgeColor);
}

void TextUI::StartStamp(
	const std::string& text,
	float interval,
	float stampFrame,
	float startScale)
{
	//スタンプ演出を開始するための初期化処理
	m_fullText = text;
	m_interval = interval;
	m_stampFrame = stampFrame;
	m_startScale = startScale;
	m_frame = 0;
}

int TextUI::CountUTF8Chars(const std::string& str) const
{
	//UTF-8文字列の文字数を数える関数
	int count = 0;
	//string型はunsigned charの配列として扱うことができるため、各バイトをunsigned charとして処理する
	for (unsigned char byte : str)
	{
		//UTF-8の先頭バイトは0xxxxxxx、110xxxxx、1110xxxx、11110xxxのいずれかの形式をとるため、先頭バイトを見て文字数をカウントする
		//0xCO = 11000000(上位2ビットを取り出す)
		//0x80 = 10000000(先頭バイトでないことを示すビットパターン)
		//つまり上位2ビットで先頭バイトかどうかを判定している
		if((byte & 0xC0) != 0x80)
		{
			count++;
		}
	}
	return count;
}
