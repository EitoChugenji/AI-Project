#include "RankingScene.h"

#include "GameConfig.h"
#include "GameStrings.h"
#include "GameSession.h"
#include "RankingManager.h"
#include "UiMouse.h"
#include "DxLib.h"

namespace
{
	static const int BTN_LEFT = 440;
	static const int BTN_RIGHT = 840;
	static const int BTN_BACK_TOP = 640;
	static const int BTN_BACK_BOTTOM = 700;
	static const int BTN_CLEAR_LEFT = 1000;
	static const int BTN_CLEAR_RIGHT = 1220;
	static const int BTN_CLEAR_TOP = 40;
	static const int BTN_CLEAR_BOTTOM = 90;
	static const int BTN_TAB_NORMAL_LEFT = 420;
	static const int BTN_TAB_NORMAL_RIGHT = 620;
	static const int BTN_TAB_EASY_LEFT = 640;
	static const int BTN_TAB_EASY_RIGHT = 840;
	static const int BTN_TAB_TOP = 100;
	static const int BTN_TAB_BOTTOM = 140;
}

void RankingScene::Init()
{
	m_requestReturn = false;
	RankingManager::Load();
}

void RankingScene::Update()
{
	UiMouse::UpdateFrame();

	if (UiMouse::TryClick(BTN_LEFT, BTN_BACK_TOP, BTN_RIGHT, BTN_BACK_BOTTOM))
	{
		m_requestReturn = true;
	}
	
	if (UiMouse::TryClick(BTN_CLEAR_LEFT, BTN_CLEAR_TOP, BTN_CLEAR_RIGHT, BTN_CLEAR_BOTTOM))
	{
		RankingManager::ClearAll();
	}
	
	if (UiMouse::TryClick(BTN_TAB_NORMAL_LEFT, BTN_TAB_TOP, BTN_TAB_NORMAL_RIGHT, BTN_TAB_BOTTOM))
	{
		RankingManager::SetViewDifficulty(GameDifficulty::Normal);
	}
	if (UiMouse::TryClick(BTN_TAB_EASY_LEFT, BTN_TAB_TOP, BTN_TAB_EASY_RIGHT, BTN_TAB_BOTTOM))
	{
		RankingManager::SetViewDifficulty(GameDifficulty::Easy);
	}
}

void RankingScene::Draw()
{
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		const int blue = 18 + (y * 40 / SCREEN_HEIGHT);
		DrawLine(0, y, SCREEN_WIDTH, y, GetColor(10, 12, blue + 24));
	}

	SetFontSize(44);
	const wchar_t* diffStr = RankingManager::GetViewDifficulty() == GameDifficulty::Easy ? STR_DIFF_EASY : STR_DIFF_NORMAL;
	DrawFormatString(SCREEN_WIDTH / 2 - 220, 30, GetColor(255, 235, 190), L"%s %s", STR_RANKING_TITLE, diffStr);

	SetFontSize(20);
	UiMouse::DrawButton(BTN_TAB_NORMAL_LEFT, BTN_TAB_TOP, BTN_TAB_NORMAL_RIGHT, BTN_TAB_BOTTOM, STR_DIFF_NORMAL);
	UiMouse::DrawButton(BTN_TAB_EASY_LEFT, BTN_TAB_TOP, BTN_TAB_EASY_RIGHT, BTN_TAB_BOTTOM, STR_DIFF_EASY);

	SetFontSize(22);
	DrawFormatString(280, 160, GetColor(180, 190, 220), STR_RANKING_COL_RANK);
	DrawFormatString(360, 160, GetColor(180, 190, 220), STR_RANKING_COL_NAME);
	DrawFormatString(560, 160, GetColor(180, 190, 220), STR_RANKING_COL_SCORE);
	DrawFormatString(700, 160, GetColor(180, 190, 220), STR_RANKING_COL_COMBO);

	const int entryCount = RankingManager::GetEntryCount();
	const int highlightIndex = RankingManager::GetHighlightIndex();

	if (entryCount <= 0)
	{
		SetFontSize(26);
		DrawFormatString(
			SCREEN_WIDTH / 2 - 200,
			300,
			GetColor(160, 160, 180),
			STR_RANKING_EMPTY);
	}
	else
	{
		for (int i = 0; i < entryCount; ++i)
		{
			const RankingEntry& entry = RankingManager::GetEntry(i);
			const int rowY = 210 + i * 42;
			const bool isHighlighted = (i == highlightIndex);
			const int textColor = isHighlighted
				? GetColor(255, 240, 80)
				: GetColor(230, 230, 245);

			DrawFormatString(280, rowY, textColor, L"%2d.", i + 1);
			DrawFormatString(360, rowY, textColor, L"%s", entry.name);
			DrawFormatString(560, rowY, textColor, L"%7d", entry.score);
			DrawFormatString(700, rowY, textColor, L"x%d", entry.maxCombo);
		}
	}

	if (highlightIndex >= 0)
	{
		SetFontSize(20);
		DrawFormatString(
			SCREEN_WIDTH / 2 - 180,
			600,
			GetColor(255, 230, 90),
			STR_RANKING_NEW_ENTRY);
	}

	UiMouse::DrawButton(BTN_LEFT, BTN_BACK_TOP, BTN_RIGHT, BTN_BACK_BOTTOM, STR_BTN_BACK);
	
	SetFontSize(20);
	UiMouse::DrawButton(BTN_CLEAR_LEFT, BTN_CLEAR_TOP, BTN_CLEAR_RIGHT, BTN_CLEAR_BOTTOM, STR_BTN_CLEAR_DATA);
	// カスタムカーソル
	UiMouse::DrawCursor(GameSession::GetCursorRadius(), false);
}

SceneID RankingScene::GetNextSceneID() const
{
	if (m_requestReturn)
	{
		return RankingManager::GetReturnScene();
	}
	return SceneID::None;
}
