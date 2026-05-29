#pragma once

#include "GameConfig.h"

// ひらがな50音キーボードによる名前入力コンポーネント
// ResultScene がインスタンスを持ち、ランキング登録名の入力に使用する
class NameInput
{
public:
	// 入力バッファをクリアして初期状態に戻す
	void Reset();

	// キーボードのヒット判定と文字追加・削除を処理する
	// 入力: パネル左上座標（ボタン配置の基準点）
	// 副作用: m_buffer、m_length が更新される
	void Update(int panelX, int panelY);

	// キーボードUIとバッファ内容を描画する
	// 入力: 表示座標、ラベル色、テキスト色
	void Draw(int x, int y, int labelColor, int textColor) const;

	// 入力済み文字列を返す（ヌル終端保証）
	const wchar_t* GetText() const;

	// 1文字以上入力されているか
	bool HasText() const;

private:
	// バッファ末尾に1文字追加する（上限超えは無視）
	void AppendChar(wchar_t ch);

	// 指定領域のクリックを UiMouse に委譲する（キー別判定用ラッパー）
	bool TryClickKey(int left, int top, int right, int bottom);

	wchar_t m_buffer[RANKING_NAME_MAX_LEN + 1];
	int     m_length;
};