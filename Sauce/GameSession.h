#pragma once

enum class GameDifficulty
{
	Normal,
	Easy,
	Hell
};

// ゲーム全体の難易度設定、ユーザーの入力設定（マウス感度、カーソルサイズ）、およびデバッグ設定を保持するセッションマネージャー
class GameSession
{
public:
	// ゲームセッション内の一時的なプレイ状態をリセットする
	// 入力: なし
	// 出力: なし
	// 副作用: 直近スコアや終了フラグなどの一時状態が初期値に戻る
	static void Reset();

	// ゲーム終了時のプレイ結果を保存する
	// 入力: score(獲得したスコア), maxCombo(最大コンボ数), isGameOver(時間切れ等でのゲームオーバー判定)
	// 出力: なし
	// 副作用: 直近セッションの結果データが上書きされる
	static void SetLastResult(int score, int maxCombo, bool isGameOver);

	// 直近のセッションで獲得したスコアを取得する
	// 入力: なし
	// 出力: 直近のスコア値
	// 副作用: なし
	static int GetLastScore();

	// 直近のセッションでの最大コンボ数を取得する
	// 入力: なし
	// 出力: 最大コンボ数
	// 副作用: なし
	static int GetLastMaxCombo();

	// 直近のゲームプレイがゲームオーバーで終了したかを取得する
	// 入力: なし
	// 出力: ゲームオーバー終了の場合はtrue、それ以外はfalse
	// 副作用: なし
	static bool IsGameOver();

	// アプリケーション全体の終了リクエストをセットする
	// 入力: なし
	// 出力: なし
	// 副作用: アプリケーション終了フラグがアクティブになる
	static void RequestQuit();

	// 終了リクエストが立っているかを検知し、消費する
	// 入力: なし
	// 出力: 終了リクエストが存在する場合はtrue、それ以外はfalse
	// 副作用: 終了リクエストフラグが消費（リセット）される
	static bool ConsumeQuitRequest();

	// ゲームの難易度を設定する
	// 入力: diff(設定する難易度列挙型)
	// 出力: なし
	// 副作用: ゲーム設定の難易度値が変更される
	static void SetDifficulty(GameDifficulty diff);

	// 現在設定されているゲーム難易度を取得する
	// 入力: なし
	// 出力: 難易度列挙型
	// 副作用: なし
	static GameDifficulty GetDifficulty();

	// マウス感度係数を取得する
	// 入力: なし
	// 出力: 感度値(デフォルト1.0f)
	// 副作用: なし
	static float GetMouseSensitivity();

	// マウス感度係数を設定する（最小・最大に制限）
	// 入力: value(設定する感度)
	// 出力: なし
	// 副作用: 感度値が0.1fから3.0fの範囲内に制限されて設定される
	static void SetMouseSensitivity(float value);

	// カスタムカーソルの半径を取得する
	// 入力: なし
	// 出力: 半径ピクセル値
	// 副作用: なし
	static float GetCursorRadius();

	// カスタムカーソルの半径を設定する（最小・最大に制限）
	// 入力: value(設定する半径)
	// 出力: なし
	// 副作用: 半径値が4.0fから60.0fの範囲内に制限されて設定される
	static void SetCursorRadius(float value);

	// ゲーム設定情報を外部ファイルから読み込む
	// 入力: なし
	// 出力: なし
	// 副作用: config.datが存在する場合、マウス感度とカーソル半径が上書きされる
	static void LoadConfig();

	// ゲーム設定情報を外部ファイルに保存する
	// 入力: なし
	// 出力: なし
	// 副作用: config.datが新規作成または上書きされる
	static void SaveConfig();

	// デバッグ機能：デバッグモードの有効状態のゲッターおよびセッター
	// 入力: なし（ゲッター）、value(設定する状態)（セッター）
	// 出力: デバッグモードが有効な場合はtrue、それ以外はfalse（ゲッター）
	// 副作用: デバッグモードの状態が書き換わる（セッター）
	static bool GetDebugModeEnabled();
	static void SetDebugModeEnabled(bool value);

	// デバッグ機能：オートクリック機能の有効状態のゲッターおよびセッター
	// 入力: なし（ゲッター）、value(設定する状態)（セッター）
	// 出力: 有効な場合はtrue（ゲッター）
	// 副作用: オートクリック状態が書き換わる（セッター）
	static bool GetDebugAutoClick();
	static void SetDebugAutoClick(bool value);

	// デバッグ機能：コンボが切れない機能の有効状態のゲッターおよびセッター
	// 入力: なし（ゲッター）、value(設定する状態)（セッター）
	// 出力: 有効な場合はtrue（ゲッター）
	// 副作用: コンボ維持状態が書き換わる（セッター）
	static bool GetDebugNoComboBreak();
	static void SetDebugNoComboBreak(bool value);

	// デバッグ機能：トラップペナルティ無効化機能の有効状態のゲッターおよびセッター
	// 入力: なし（ゲッター）、value(設定する状態)（セッター）
	// 出力: 有効な場合はtrue（ゲッター）
	// 副作用: トラップペナルティ無効化状態が書き換わる（セッター）
	static bool GetDebugNoTrapPenalty();
	static void SetDebugNoTrapPenalty(bool value);

	// デバッグ機能：プレイ制限時間無限化機能の有効状態のゲッターおよびセッター
	// 入力: なし（ゲッター）、value(設定する状態)（セッター）
	// 出力: 有効な場合はtrue（ゲッター）
	// 副作用: 制限時間無限化状態が書き換わる（セッター）
	static bool GetDebugInfiniteTime();
	static void SetDebugInfiniteTime(bool value);

private:
	static float s_mouseSensitivity;
	static float s_cursorRadius;
	static int s_lastScore;
	static int s_lastMaxCombo;
	static bool s_isGameOver;
	static bool s_quitRequested;
	static GameDifficulty s_difficulty;

	static bool s_debugModeEnabled;
	static bool s_debugAutoClick;
	static bool s_debugNoComboBreak;
	static bool s_debugNoTrapPenalty;
	static bool s_debugInfiniteTime;
};
