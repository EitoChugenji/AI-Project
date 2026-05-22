#pragma once

#include "GameConfig.h"
#include "GameSession.h"
#include "SceneBase.h"

struct RankingEntry
{
	wchar_t name[RANKING_NAME_MAX_LEN + 1];
	int score;
	int maxCombo;
	unsigned int saveId;
};

class RankingManager
{
public:
	static void Load();
	static bool SaveCurrentScore(const wchar_t* name, int score, int maxCombo);
	static int GetEntryCount();
	static const RankingEntry& GetEntry(int index);
	static int GetHighlightIndex();
	static void ClearHighlight();
	static void ClearAll();

	static void SetReturnScene(SceneID sceneID);
	static SceneID GetReturnScene();

	static void SetViewDifficulty(GameDifficulty diff);
	static GameDifficulty GetViewDifficulty();

private:
	static const wchar_t* GetFileName(GameDifficulty diff);
	static void SortEntries();
	static void SaveToFile();
	static bool LoadFromFile();

	static RankingEntry s_entries[RANKING_MAX_ENTRIES];
	static int s_entryCount;
	static int s_highlightIndex;
	static unsigned int s_nextSaveId;
	static SceneID s_returnScene;
	static GameDifficulty s_viewDifficulty;
};
