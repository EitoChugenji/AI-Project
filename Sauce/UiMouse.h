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

	// カスタムカーソル描画（ゲーム中に OSカーソルの代わりに描く）
	// radius: 表示する円の半径（ピクセル）
	// showHitArea: 当たり判定円を重ねて表示するか
	static void DrawCursor(float radius, bool showHitArea = true);
};
