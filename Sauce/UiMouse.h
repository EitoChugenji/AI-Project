#pragma once

// マウス入力の取得とUIボタン・カーソルの描画を行うユーティリティクラス
// 全メンバは static であり、フレームごとに UpdateFrame() を1回呼ぶ必要がある
class UiMouse
{
public:
	// フレーム先頭で1回呼ぶ。左クリックの立ち上がりを検出してフレームをリセットする
	static void UpdateFrame();

	// 指定領域内にマウスがあれば左クリックを消費して true を返す
	// 1フレームに1クリックしか消費しない（最初にヒットした領域が優先）
	static bool TryClick(int left, int top, int right, int bottom);

	// 指定領域内にマウスが重なっているかを返す（クリック消費なし）
	static bool IsOver(int left, int top, int right, int bottom);

	// 指定領域にボタンを描画する。ホバー・ハイライト状態で色が変化する
	// 入力: 矩形領域、ラベル文字列、enabled（非活性化フラグ）、highlighted（選択中フラグ）
	static void DrawButton(
		int left,
		int top,
		int right,
		int bottom,
		const wchar_t* label,
		bool enabled = true,
		bool highlighted = false);

	// カスタムカーソルを描画する（OS標準カーソルの代わりに使用）
	// 入力: radius=当たり判定円の半径(px)、showHitArea=判定円を重ねて表示するか
	static void DrawCursor(float radius, bool showHitArea = true);
};