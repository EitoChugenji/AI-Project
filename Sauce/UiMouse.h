#pragma once

class UiMouse
{
public:
	static void UpdateFrame();
	static bool TryClick(int left, int top, int right, int bottom);
	static bool IsOver(int left, int top, int right, int bottom);
	static void DrawButton(
		int left,
		int top,
		int right,
		int bottom,
		const wchar_t* label,
		bool enabled = true);
};
