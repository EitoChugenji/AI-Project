#include "UiMouse.h"

#include "DxLib.h"

namespace
{
	// フレーム単位のクリック状態を管理する静的変数
	bool s_clickAvailable = false;   // このフレームにクリックが発生した
	bool s_clickConsumed  = false;   // クリックが既に消費済み
	int  s_prevMouseInput = 0;       // 前フレームのマウス入力状態（立ち上がり検出用）
}

void UiMouse::UpdateFrame()
{
	const int mouseInput = GetMouseInput();
	// 前フレームで押されておらず、このフレームで押されたとき立ち上がりとみなす
	const bool clicked =
		(mouseInput & MOUSE_INPUT_LEFT) != 0 &&
		(s_prevMouseInput & MOUSE_INPUT_LEFT) == 0;
	s_prevMouseInput = mouseInput;

	s_clickAvailable = clicked;
	s_clickConsumed  = false;
}

bool UiMouse::IsOver(int left, int top, int right, int bottom)
{
	int mouseX = 0;
	int mouseY = 0;
	GetMousePoint(&mouseX, &mouseY);
	return mouseX >= left && mouseX <= right && mouseY >= top && mouseY <= bottom;
}

bool UiMouse::TryClick(int left, int top, int right, int bottom)
{
	// 同一フレームで複数領域が重なっていても1つだけ消費する
	if (!s_clickAvailable || s_clickConsumed)
	{
		return false;
	}
	if (!IsOver(left, top, right, bottom))
	{
		return false;
	}

	s_clickConsumed = true;
	return true;
}

void UiMouse::DrawButton(
	int left,
	int top,
	int right,
	int bottom,
	const wchar_t* label,
	bool enabled,
	bool highlighted)
{
	const bool hovered = enabled && IsOver(left, top, right, bottom);
	int bodyColor;
	int borderColor;
	if (highlighted)
	{
		// 選択中ボタンはゴールド系で目立たせる
		bodyColor   = hovered ? GetColor(110, 95, 45) : GetColor(160, 130, 60);
		borderColor = GetColor(255, 235, 170);
	}
	else
	{
		// 通常ボタンはホバー有無と活性状態で色を切り替える
		bodyColor = enabled
			? (hovered ? GetColor(90, 110, 160) : GetColor(60, 75, 120))
			: GetColor(45, 45, 55);
		borderColor = enabled
			? GetColor(200, 210, 255)
			: GetColor(90, 90, 100);
	}

	DrawBox(left, top, right, bottom, bodyColor, TRUE);
	DrawBox(left, top, right, bottom, borderColor, FALSE);

	// ラベルをボタン中央に揃えて描画する
	SetFontSize(22);
	const int textWidth  = GetDrawStringWidth(label, -1);
	const int textHeight = GetFontSize();
	const int textColor  = highlighted ? GetColor(255, 255, 220) : GetColor(255, 255, 255);
	DrawFormatString(
		left + (right - left - textWidth) / 2,
		top  + (bottom - top - textHeight) / 2,
		textColor,
		label);
}

void UiMouse::DrawCursor(float radius, bool showHitArea)
{
	int mx = 0, my = 0;
	GetMousePoint(&mx, &my);

	const int r = static_cast<int>(radius);

	// 当たり判定エリア（薄い半透明の円）を一定サイズ以上のとき表示する
	if (showHitArea && r > 10)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 55);
		DrawCircle(mx, my, r, GetColor(100, 200, 255), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawCircle(mx, my, r, GetColor(80, 180, 255), FALSE);
	}

	// カーソル本体（中心の白丸）
	DrawCircle(mx, my, 5, GetColor(255, 255, 255), TRUE);
	DrawCircle(mx, my, 5, GetColor(0, 0, 0), FALSE);

	// カーソルの十字線
	const int crossLen = 12;
	DrawLine(mx - crossLen, my, mx - 6, my, GetColor(255, 255, 255));
	DrawLine(mx + 6,        my, mx + crossLen, my, GetColor(255, 255, 255));
	DrawLine(mx, my - crossLen, mx, my - 6, GetColor(255, 255, 255));
	DrawLine(mx, my + 6,        mx, my + crossLen, GetColor(255, 255, 255));
}