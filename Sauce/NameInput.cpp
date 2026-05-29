#include "NameInput.h"

#include "GameStrings.h"
#include "UiMouse.h"
#include "DxLib.h"

namespace
{
	// 50音表に合わせた文字配列。空白マスには L' ' を配置して行列の整合性を保つ
	static const wchar_t HIRAGANA_TABLE[] =
		L"\u308F\u3089\u3084\u307E\u306F\u306A\u305F\u3055\u304B\u3042" // わらやまはなたさかあ
		L"\u3092\u308A \u307F\u3072\u306B\u3061\u3057\u304D\u3044"      // をり(空)みひにちしきい
		L"\u3093\u308B\u3086\u3080\u3075\u306C\u3064\u3059\u304F\u3046" // んるゆむふぬつすくう
		L" \u308C \u3081\u3078\u306D\u3066\u305B\u3051\u3048"           // (空)れ(空)めへねてせけえ
		L" \u308D\u3088\u3082\u307B\u306E\u3068\u305D\u3053\u304A";     // (空)ろよもほのとそこお

	static const int HIRAGANA_COUNT = 50;
	static const int KEY_COLS = 10;
	static const int KEY_W    = 44;
	static const int KEY_H    = 34;
	static const int KEY_GAP  = 3;
}

void NameInput::Reset()
{
	m_buffer[0] = L'\0';
	m_length = 0;
}

void NameInput::AppendChar(wchar_t ch)
{
	// 上限に達している場合は追加しない
	if (m_length >= RANKING_NAME_MAX_LEN)
	{
		return;
	}
	m_buffer[m_length++] = ch;
	m_buffer[m_length]   = L'\0';
}

void NameInput::Update(int panelX, int panelY)
{
	const int keysTop  = panelY + 120;
	const int keysLeft = panelX;

	for (int i = 0; i < HIRAGANA_COUNT; ++i)
	{
		// 空白マスの場合はクリック判定をスキップする
		if (HIRAGANA_TABLE[i] == L' ')
		{
			continue;
		}

		const int col   = i % KEY_COLS;
		const int row   = i / KEY_COLS;
		const int left  = keysLeft + col * (KEY_W + KEY_GAP);
		const int top   = keysTop  + row * (KEY_H + KEY_GAP);
		const int right  = left + KEY_W;
		const int bottom = top  + KEY_H;

		if (TryClickKey(left, top, right, bottom))
		{
			AppendChar(HIRAGANA_TABLE[i]);
		}
	}

	// バックスペースボタン（50音の下に配置）
	const int backRow    = (HIRAGANA_COUNT + KEY_COLS - 1) / KEY_COLS;
	const int backTop    = keysTop  + backRow * (KEY_H + KEY_GAP);
	const int backLeft   = keysLeft;
	const int backRight  = backLeft + KEY_W * 2 + KEY_GAP;
	const int backBottom = backTop  + KEY_H;

	if (UiMouse::TryClick(backLeft, backTop, backRight, backBottom) && m_length > 0)
	{
		--m_length;
		m_buffer[m_length] = L'\0';
	}

	// 全消去ボタン
	const int clearLeft  = backRight + KEY_GAP;
	const int clearRight = clearLeft + KEY_W * 2 + KEY_GAP;
	if (UiMouse::TryClick(clearLeft, backTop, clearRight, backBottom))
	{
		m_length     = 0;
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
	// バッファが空のとき1文字分のスペースを描画して高さを確保する
	DrawFormatString(x, y + 36, textColor, L"%s", m_buffer[0] != L'\0' ? m_buffer : L" ");
	SetFontSize(18);
	DrawFormatString(x, y + 72, GetColor(150, 150, 170), STR_NAME_INPUT_HINT);

	const int keysTop  = y + 120;
	const int keysLeft = x;

	for (int i = 0; i < HIRAGANA_COUNT; ++i)
	{
		// 空白マスはボタンを描画しない
		if (HIRAGANA_TABLE[i] == L' ')
		{
			continue;
		}

		const int col    = i % KEY_COLS;
		const int row    = i / KEY_COLS;
		const int left   = keysLeft + col * (KEY_W + KEY_GAP);
		const int top    = keysTop  + row * (KEY_H + KEY_GAP);
		const int right  = left + KEY_W;
		const int bottom = top  + KEY_H;

		wchar_t label[2] = { HIRAGANA_TABLE[i], L'\0' };
		UiMouse::DrawButton(left, top, right, bottom, label);
	}

	const int backRow    = (HIRAGANA_COUNT + KEY_COLS - 1) / KEY_COLS;
	const int backTop    = keysTop  + backRow * (KEY_H + KEY_GAP);
	const int backLeft   = keysLeft;
	const int backRight  = backLeft + KEY_W * 2 + KEY_GAP;
	const int backBottom = backTop  + KEY_H;

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