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