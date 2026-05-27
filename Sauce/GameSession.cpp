#include "GameSession.h"
#include <cstdio>

float GameSession::s_mouseSensitivity = 1.0f;
float GameSession::s_cursorRadius = 8.0f;
int GameSession::s_lastScore = 0;
int GameSession::s_lastMaxCombo = 0;
bool GameSession::s_isGameOver = false;
bool GameSession::s_quitRequested = false;
GameDifficulty GameSession::s_difficulty = GameDifficulty::Normal;

bool GameSession::s_debugModeEnabled = false;
bool GameSession::s_debugAutoClick = false;
bool GameSession::s_debugNoComboBreak = false;
bool GameSession::s_debugNoTrapPenalty = false;
bool GameSession::s_debugInfiniteTime = false;

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

float GameSession::GetMouseSensitivity()
{
	return s_mouseSensitivity;
}

void GameSession::SetMouseSensitivity(float value)
{
	s_mouseSensitivity = value;
	if (s_mouseSensitivity < 0.1f) s_mouseSensitivity = 0.1f;
	if (s_mouseSensitivity > 3.0f) s_mouseSensitivity = 3.0f;
}

float GameSession::GetCursorRadius()
{
	return s_cursorRadius;
}

void GameSession::SetCursorRadius(float value)
{
	s_cursorRadius = value;
	if (s_cursorRadius < 4.0f)  s_cursorRadius = 4.0f;
	if (s_cursorRadius > 60.0f) s_cursorRadius = 60.0f;
}

void GameSession::LoadConfig()
{
	FILE* fp = nullptr;
	if (fopen_s(&fp, "config.dat", "r") == 0 && fp != nullptr)
	{
		float sens = 1.0f;
		float crad = 8.0f;
		fscanf_s(fp, "%f %f", &sens, &crad);
		SetMouseSensitivity(sens);
		SetCursorRadius(crad);
		fclose(fp);
	}
}

void GameSession::SaveConfig()
{
	FILE* fp = nullptr;
	if (fopen_s(&fp, "config.dat", "w") == 0 && fp != nullptr)
	{
		fprintf_s(fp, "%.2f %.2f", s_mouseSensitivity, s_cursorRadius);
		fclose(fp);
	}
}

bool GameSession::GetDebugModeEnabled() { return s_debugModeEnabled; }
void GameSession::SetDebugModeEnabled(bool value) { s_debugModeEnabled = value; }

bool GameSession::GetDebugAutoClick() { return s_debugAutoClick; }
void GameSession::SetDebugAutoClick(bool value) { s_debugAutoClick = value; }

bool GameSession::GetDebugNoComboBreak() { return s_debugNoComboBreak; }
void GameSession::SetDebugNoComboBreak(bool value) { s_debugNoComboBreak = value; }

bool GameSession::GetDebugNoTrapPenalty() { return s_debugNoTrapPenalty; }
void GameSession::SetDebugNoTrapPenalty(bool value) { s_debugNoTrapPenalty = value; }

bool GameSession::GetDebugInfiniteTime() { return s_debugInfiniteTime; }
void GameSession::SetDebugInfiniteTime(bool value) { s_debugInfiniteTime = value; }
