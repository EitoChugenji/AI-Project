#include "ResultScene.h"

#include "GameConfig.h"
#include "GameStrings.h"
#include "GameSession.h"
#include "RankingManager.h"
#include "UiMouse.h"
#include "DxLib.h"

namespace
{
	static const int BTN_LEFT = 340;
	static const int BTN_RIGHT = 940;
	static const int BTN_SAVE_TOP = 580;
	static const int BTN_SAVE_BOTTOM = 630;
	static const int BTN_TITLE_TOP = 640;
	static const int BTN_TITLE_BOTTOM = 690;
	static const int NAME_PANEL_X = SCREEN_WIDTH / 2 - 260;
	static const int NAME_PANEL_Y = 220;
}

void ResultScene::Init()
{
	m_requestGoRanking = false;
	m_requestGoTitle = false;
	m_nameInput.Reset();
}

void ResultScene::Update()
{
	UiMouse::UpdateFrame();
	m_nameInput.Update(NAME_PANEL_X, NAME_PANEL_Y);

	if (UiMouse::TryClick(BTN_LEFT, BTN_SAVE_TOP, BTN_RIGHT, BTN_SAVE_BOTTOM)
		&& m_nameInput.HasText())
	{
		const bool saved = RankingManager::SaveCurrentScore(
			m_nameInput.GetText(),
			GameSession::GetLastScore(),
			GameSession::GetLastMaxCombo());
		if (saved)
		{
			RankingManager::SetReturnScene(SceneID::Title);
			m_requestGoRanking = true;
		}
	}

	if (UiMouse::TryClick(BTN_LEFT, BTN_TITLE_TOP, BTN_RIGHT, BTN_TITLE_BOTTOM))
	{
		m_requestGoTitle = true;
	}
}

void ResultScene::Draw()
{
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		const int blue = 20 + (y * 45 / SCREEN_HEIGHT);
		DrawLine(0, y, SCREEN_WIDTH, y, GetColor(12, 10, blue + 20));
	}

	SetFontSize(48);
	DrawFormatString(SCREEN_WIDTH / 2 - 160, 40, GetColor(255, 230, 180), STR_RESULT_TITLE);

	SetFontSize(32);
	DrawFormatString(
		SCREEN_WIDTH / 2 - 200,
		100,
		GetColor(255, 245, 200),
		STR_RESULT_SCORE,
		GameSession::GetLastScore());

	SetFontSize(26);
	DrawFormatString(
		SCREEN_WIDTH / 2 - 180,
		140,
		GetColor(200, 220, 255),
		STR_RESULT_COMBO,
		GameSession::GetLastMaxCombo());

	SetFontSize(24);
	if (GameSession::IsGameOver())
	{
		DrawFormatString(SCREEN_WIDTH / 2 - 320, 180, GetColor(255, 150, 150), STR_RESULT_GAMEOVER);
	}
	else
	{
		DrawFormatString(SCREEN_WIDTH / 2 - 260, 180, GetColor(180, 180, 210), STR_RESULT_MESSAGE);
	}

	m_nameInput.Draw(
		NAME_PANEL_X,
		NAME_PANEL_Y,
		GetColor(255, 240, 200),
		GetColor(255, 255, 255));

	UiMouse::DrawButton(BTN_LEFT, BTN_SAVE_TOP, BTN_RIGHT, BTN_SAVE_BOTTOM,
		STR_BTN_SAVE_RANKING,
		m_nameInput.HasText());
	UiMouse::DrawButton(BTN_LEFT, BTN_TITLE_TOP, BTN_RIGHT, BTN_TITLE_BOTTOM, STR_BTN_TO_TITLE);
	// カスタムカーソルを最前面に描画（当たり判定円はリザルトで非表示）
	UiMouse::DrawCursor(GameSession::GetCursorRadius(), false);
}

SceneID ResultScene::GetNextSceneID() const
{
	if (m_requestGoRanking)
	{
		return SceneID::Ranking;
	}
	if (m_requestGoTitle)
	{
		return SceneID::Title;
	}
	return SceneID::None;
}
