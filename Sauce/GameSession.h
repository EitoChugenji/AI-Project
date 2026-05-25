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

private:
	static int s_lastScore;
	static int s_lastMaxCombo;
	static bool s_isGameOver;
	static bool s_quitRequested;
	static GameDifficulty s_difficulty;
};
