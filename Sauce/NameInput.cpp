#include "NameInput.h"

#include "GameStrings.h"
#include "UiMouse.h"
#include "DxLib.h"

namespace
{
	static const wchar_t HIRAGANA_TABLE[] =
		L"\u3042\u3044\u3046\u3048\u304A"   // あいうえお
		L"\u304B\u304D\u304F\u3051\u3053"   // かきくけこ
		L"\u3055\u3057\u3059\u305B\u305D"   // さしすせそ
		L"\u305F\u3061\u3064\u3066\u3068"   // たちつてと
		L"\u306A\u306B\u306C\u306D\u306E"   // なにぬねの
		L"\u306F\u3072\u3075\u3078\u307B"   // はひふへほ
		L"\u307E\u307F\u3080\u3081\u3082"   // まみむめも
		L"\u3084\u3086\u3088"               // やゆよ
		L"\u3089\u308A\u308B\u308C\u308D"   // らりるれろ
		L"\u308F\u3092\u3093";              // わをん

	static const int HIRAGANA_COUNT = 46;
	static const int KEY_COLS = 10;
	static const int KEY_W = 44;
	static const int KEY_H = 34;
	static const int KEY_GAP = 3;
}

void NameInput::Reset()
{
	m_buffer[0] = L'\0';
	m_length = 0;
}

void NameInput::AppendChar(wchar_t ch)
{
	if (m_length >= RANKING_NAME_MAX_LEN)
	{
		return;
	}
	m_buffer[m_length++] = ch;
	m_buffer[m_length] = L'\0';
}

void NameInput::Update(int panelX, int panelY)
{
	const int keysTop = panelY + 120;
	const int keysLeft = panelX;

	for (int i = 0; i < HIRAGANA_COUNT; ++i)
	{
		const int col = i % KEY_COLS;
		const int row = i / KEY_COLS;
		const int left = keysLeft + col * (KEY_W + KEY_GAP);
		const int top = keysTop + row * (KEY_H + KEY_GAP);
		const int right = left + KEY_W;
		const int bottom = top + KEY_H;

		if (TryClickKey(left, top, right, bottom))
		{
			AppendChar(HIRAGANA_TABLE[i]);
		}
	}

	const int backRow = (HIRAGANA_COUNT + KEY_COLS - 1) / KEY_COLS;
	const int backTop = keysTop + backRow * (KEY_H + KEY_GAP);
	const int backLeft = keysLeft;
	const int backRight = backLeft + KEY_W * 2 + KEY_GAP;
	const int backBottom = backTop + KEY_H;

	if (UiMouse::TryClick(backLeft, backTop, backRight, backBottom) && m_length > 0)
	{
		--m_length;
		m_buffer[m_length] = L'\0';
	}

	const int clearLeft = backRight + KEY_GAP;
	const int clearRight = clearLeft + KEY_W * 2 + KEY_GAP;
	if (UiMouse::TryClick(clearLeft, backTop, clearRight, backBottom))
	{
		m_length = 0;
		m_buffer[0] = L'\0';
	}
}

bool NameInput::TryClickKey(int left, int top, int right, int bottom)
{
	return UiMouse::TryClick(left, top, right, bottom);
}

void NameInput::Draw(int x, int y, int labelColor, int textColor) const
{
	SetFontSize(24);
	DrawFormatString(x, y, labelColor, STR_NAME_INPUT_LABEL);
	DrawFormatString(x, y + 36, textColor, L"%s", m_buffer[0] != L'\0' ? m_buffer : L" ");
	SetFontSize(18);
	DrawFormatString(x, y + 72, GetColor(150, 150, 170), STR_NAME_INPUT_HINT);

	const int keysTop = y + 120;
	const int keysLeft = x;

	for (int i = 0; i < HIRAGANA_COUNT; ++i)
	{
		const int col = i % KEY_COLS;
		const int row = i / KEY_COLS;
		const int left = keysLeft + col * (KEY_W + KEY_GAP);
		const int top = keysTop + row * (KEY_H + KEY_GAP);
		const int right = left + KEY_W;
		const int bottom = top + KEY_H;

		wchar_t label[2] = { HIRAGANA_TABLE[i], L'\0' };
		UiMouse::DrawButton(left, top, right, bottom, label);
	}

	const int backRow = (HIRAGANA_COUNT + KEY_COLS - 1) / KEY_COLS;
	const int backTop = keysTop + backRow * (KEY_H + KEY_GAP);
	const int backLeft = keysLeft;
	const int backRight = backLeft + KEY_W * 2 + KEY_GAP;
	const int backBottom = backTop + KEY_H;

	UiMouse::DrawButton(backLeft, backTop, backRight, backBottom, STR_NAME_KEY_BACK);
	UiMouse::DrawButton(
		backRight + KEY_GAP,
		backTop,
		backRight + KEY_GAP + KEY_W * 2 + KEY_GAP,
		backBottom,
		STR_NAME_KEY_CLEAR);
}

const wchar_t* NameInput::GetText() const
{
	return m_buffer;
}

bool NameInput::HasText() const
{
	return m_length > 0;
}
