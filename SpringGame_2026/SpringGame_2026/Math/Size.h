#pragma once

struct Size
{
	Size() = default;
	Size(int width, int height) :
		m_width(width), m_height(height) {
	}

	//演算子オーバーロード
	bool operator==(const Size& other) const
	{
		//幅と高さが等しいか
		return m_width == other.m_width && m_height == other.m_height;
	}

	bool operator!=(const Size& other) const
	{
		//幅と高さのどちらかが等しくないか
		return m_width != other.m_width || m_height != other.m_height;
	}

	//幅と高さ
	int m_width = 0;
	int m_height = 0;
};

