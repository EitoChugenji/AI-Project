#include "UiMouse.h"

#include "DxLib.h"

namespace
{
	bool s_clickAvailable = false;
	bool s_clickConsumed = false;
	int s_prevMouseInput = 0;
}

void UiMouse::UpdateFrame()
{
	const int mouseInput = GetMouseInput();
	const bool clicked =
		(mouseInput & MOUSE_INPUT_LEFT) != 0 &&
		(s_prevMouseInput & MOUSE_INPUT_LEFT) == 0;
	s_prevMouseInput = mouseInput;

	s_clickAvailable = clicked;
	s_clickConsumed = false;
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
	bool enabled)
{
	const bool hovered = enabled && IsOver(left, top, right, bottom);
	const int bodyColor = enabled
		? (hovered ? GetColor(90, 110, 160) : GetColor(60, 75, 120))
		: GetColor(45, 45, 55);
	const int borderColor = enabled
		? GetColor(200, 210, 255)
		: GetColor(90, 90, 100);

	DrawBox(left, top, right, bottom, bodyColor, TRUE);
	DrawBox(left, top, right, bottom, borderColor, FALSE);

	SetFontSize(22);
	const int textWidth = GetDrawStringWidth(label, -1);
	const int textHeight = GetFontSize();
	DrawFormatString(
		left + (right - left - textWidth) / 2,
		top + (bottom - top - textHeight) / 2,
		GetColor(255, 255, 255),
		label);
}
