#pragma once

enum class GameDifficulty
{
	Normal,
	Easy,
	Hell
};

class GameSession
{
public:
	static void Reset();
	static void SetLastResult(int score, int maxCombo, bool isGameOver);
	static int GetLastScore();
	static int GetLastMaxCombo();
	static bool IsGameOver();

	static void RequestQuit();
	static bool ConsumeQuitRequest();

	static void SetDifficulty(GameDifficulty diff);
	static GameDifficulty GetDifficulty();

	static float GetMouseSensitivity();
	static void SetMouseSensitivity(float value);

	static float GetCursorRadius();
	static void SetCursorRadius(float value);

	// デバッグモード設定
	static bool GetDebugModeEnabled();
	static void SetDebugModeEnabled(bool value);

	static bool GetDebugAutoClick();
	static void SetDebugAutoClick(bool value);

	static bool GetDebugNoComboBreak();
	static void SetDebugNoComboBreak(bool value);

	static bool GetDebugNoTrapPenalty();
	static void SetDebugNoTrapPenalty(bool value);

	static bool GetDebugInfiniteTime();
	static void SetDebugInfiniteTime(bool value);

	static void LoadConfig();
	static void SaveConfig();

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
