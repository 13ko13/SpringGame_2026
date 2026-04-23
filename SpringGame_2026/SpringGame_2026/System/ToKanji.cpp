#include "ToKanji.h"
#include <array>

std::string ToKanji::NumToKanji(int num)
{
	//漢数字の文字列を作成する
	std::string kanji;

	//漢数字の配列
	//string_viewは文字列リテラル(複数文字の並び)を
	//効率的に扱うための型で、std::stringよりも軽量で高速に動作する
	constexpr std::array<std::string_view, 10> kanjiDigits = { "零", "一", "二", "三", "四", "五", "六", "七", "八", "九" };

	//0のときは特別に「零」とする
	if (num == 0)
	{
		kanji = kanjiDigits[num];
		return kanji;//「零」を返す
	}

	//数字が1000未満の時は百の位も処理する
	if (num < 1000)
	{
		int hundreds = num / 100;//百の位
		int tens = num % 100 / 10;//十の位
		int ones = num % 10;//一の位

		if (hundreds == 1)
		{
			kanji += "百";
		}

		//もし百の位が1以上なら「百」を追加する
		if(hundreds > 1)
		{
			kanji += kanjiDigits[hundreds];
			kanji += "百";
		}
		

		//もし十の位が1以上なら「十」を追加する
		if (tens > 0)
		{
			if (tens > 1)
			{
				kanji += kanjiDigits[tens];
			}
			kanji += "十";
		}
		if (ones > 0)
		{
			kanji += kanjiDigits[ones];
		}

		return kanji;
	}

	return kanji;
}