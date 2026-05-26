#include "RankingManager.h"

#include "DxLib.h"

#include <cstdio>
#include <cwchar>

RankingEntry RankingManager::s_entries[RANKING_MAX_ENTRIES] = {};
int RankingManager::s_entryCount = 0;
int RankingManager::s_highlightIndex = -1;
unsigned int RankingManager::s_nextSaveId = 1;
SceneID RankingManager::s_returnScene = SceneID::Title;
GameDifficulty RankingManager::s_viewDifficulty = GameDifficulty::Normal;

const wchar_t* RankingManager::GetFileName(GameDifficulty diff)
{
	if (diff == GameDifficulty::Easy) return RANKING_FILE_NAME_EASY;
	if (diff == GameDifficulty::Hell) return RANKING_FILE_NAME_HELL;
	return RANKING_FILE_NAME_NORMAL;
}

void RankingManager::Load()
{
	if (!LoadFromFile())
	{
		s_entryCount = 0;
		s_nextSaveId = 1;
	}
}

bool RankingManager::LoadFromFile()
{
	FILE* file = nullptr;
	const wchar_t* fileName = GetFileName(s_viewDifficulty);
	if (_wfopen_s(&file, fileName, L"rt, ccs=UTF-8") != 0 || file == nullptr)
	{
		return false;
	}

	int storedCount = 0;
	if (fwscanf_s(file, L"%d", &storedCount) != 1)
	{
		fclose(file);
		return false;
	}

	s_entryCount = 0;
	unsigned int maxSaveId = 0;

	for (int i = 0; i < storedCount && s_entryCount < RANKING_MAX_ENTRIES; ++i)
	{
		RankingEntry entry = {};
		unsigned int saveId = 0;
		if (fwscanf_s(
			file,
			L" %15[^\t]\t%d\t%d\t%u",
			entry.name,
			static_cast<unsigned>(RANKING_NAME_MAX_LEN + 1),
			&entry.score,
			&entry.maxCombo,
			&saveId) != 4)
		{
			break;
		}

		entry.saveId = saveId;
		if (saveId > maxSaveId)
		{
			maxSaveId = saveId;
		}

		s_entries[s_entryCount++] = entry;
	}

	fclose(file);
	s_nextSaveId = maxSaveId + 1;
	SortEntries();
	return true;
}

void RankingManager::SaveToFile()
{
	FILE* file = nullptr;
	const wchar_t* fileName = GetFileName(s_viewDifficulty);
	if (_wfopen_s(&file, fileName, L"wt, ccs=UTF-8") != 0 || file == nullptr)
	{
		return;
	}

	fwprintf(file, L"%d\n", s_entryCount);
	for (int i = 0; i < s_entryCount; ++i)
	{
		const RankingEntry& entry = s_entries[i];
		fwprintf(
			file,
			L"%s\t%d\t%d\t%u\n",
			entry.name,
			entry.score,
			entry.maxCombo,
			entry.saveId);
	}

	fclose(file);
}

void RankingManager::SortEntries()
{
	for (int i = 0; i < s_entryCount - 1; ++i)
	{
		for (int j = i + 1; j < s_entryCount; ++j)
		{
			const RankingEntry& a = s_entries[i];
			const RankingEntry& b = s_entries[j];
			const bool shouldSwap =
				b.score > a.score ||
				(b.score == a.score && b.saveId > a.saveId);
			if (shouldSwap)
			{
				const RankingEntry temp = s_entries[i];
				s_entries[i] = s_entries[j];
				s_entries[j] = temp;
			}
		}
	}
}

bool RankingManager::SaveCurrentScore(const wchar_t* name, int score, int maxCombo)
{
	if (name == nullptr || name[0] == L'\0')
	{
		return false;
	}

	s_viewDifficulty = GameSession::GetDifficulty();
	Load();

	unsigned int savedId = s_nextSaveId++;
	bool updated = false;

	for (int i = 0; i < s_entryCount; ++i)
	{
		if (wcscmp(s_entries[i].name, name) == 0)
		{
			if (score >= s_entries[i].score)
			{
				s_entries[i].score = score;
				s_entries[i].maxCombo = maxCombo;
			}
			s_entries[i].saveId = savedId;
			updated = true;
			break;
		}
	}

	if (!updated)
	{
		if (s_entryCount < RANKING_MAX_ENTRIES)
		{
			RankingEntry& entry = s_entries[s_entryCount++];
			wcscpy_s(entry.name, name);
			entry.score = score;
			entry.maxCombo = maxCombo;
			entry.saveId = savedId;
		}
		else if (score > s_entries[s_entryCount - 1].score)
		{
			RankingEntry& entry = s_entries[s_entryCount - 1];
			wcscpy_s(entry.name, name);
			entry.score = score;
			entry.maxCombo = maxCombo;
			entry.saveId = savedId;
		}
		else
		{
			return false;
		}
	}

	SortEntries();
	s_highlightIndex = -1;
	for (int i = 0; i < s_entryCount; ++i)
	{
		if (s_entries[i].saveId == savedId)
		{
			s_highlightIndex = i;
			break;
		}
	}

	SaveToFile();
	return s_highlightIndex >= 0;
}

int RankingManager::GetEntryCount()
{
	return s_entryCount;
}

const RankingEntry& RankingManager::GetEntry(int index)
{
	static RankingEntry empty = {};
	if (index < 0 || index >= s_entryCount)
	{
		return empty;
	}
	return s_entries[index];
}

int RankingManager::GetHighlightIndex()
{
	return s_highlightIndex;
}

void RankingManager::ClearHighlight()
{
	s_highlightIndex = -1;
}

void RankingManager::ClearAll()
{
	s_entryCount = 0;
	s_highlightIndex = -1;
	s_nextSaveId = 1;
	SaveToFile();
}

void RankingManager::SetReturnScene(SceneID sceneID)
{
	s_returnScene = sceneID;
}

SceneID RankingManager::GetReturnScene()
{
	return s_returnScene;
}

void RankingManager::SetViewDifficulty(GameDifficulty diff)
{
	s_viewDifficulty = diff;
	Load();
}

GameDifficulty RankingManager::GetViewDifficulty()
{
	return s_viewDifficulty;
}
