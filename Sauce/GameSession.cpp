#include "GameSession.h"

int GameSession::s_lastScore = 0;
int GameSession::s_lastMaxCombo = 0;
bool GameSession::s_isGameOver = false;
bool GameSession::s_quitRequested = false;
GameDifficulty GameSession::s_difficulty = GameDifficulty::Normal;

void GameSession::Reset()
{
	s_lastScore = 0;
	s_lastMaxCombo = 0;
	s_isGameOver = false;
	s_quitRequested = false;
}

void GameSession::SetLastResult(int score, int maxCombo, bool isGameOver)
{
	s_lastScore = score;
	s_lastMaxCombo = maxCombo;
	s_isGameOver = isGameOver;
}

int GameSession::GetLastScore()
{
	return s_lastScore;
}

int GameSession::GetLastMaxCombo()
{
	return s_lastMaxCombo;
}

bool GameSession::IsGameOver()
{
	return s_isGameOver;
}

void GameSession::RequestQuit()
{
	s_quitRequested = true;
}

bool GameSession::ConsumeQuitRequest()
{
	if (s_quitRequested)
	{
		s_quitRequested = false;
		return true;
	}
	return false;
}

void GameSession::SetDifficulty(GameDifficulty diff)
{
	s_difficulty = diff;
}

GameDifficulty GameSession::GetDifficulty()
{
	return s_difficulty;
}
