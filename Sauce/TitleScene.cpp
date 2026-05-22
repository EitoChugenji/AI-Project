#include "TitleScene.h"

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
	static const int BTN_PLAY_TOP = 470;
	static const int BTN_PLAY_BOTTOM = 520;
	static const int BTN_RANK_TOP = 535;
	static const int BTN_RANK_BOTTOM = 585;
	static const int BTN_QUIT_TOP = 600;
	static const int BTN_QUIT_BOTTOM = 650;
	
	static const int BTN_PLAY_EASY_TOP = 470;
	static const int BTN_PLAY_EASY_BOTTOM = 520;
	static const int BTN_PLAY_NORMAL_TOP = 535;
	static const int BTN_PLAY_NORMAL_BOTTOM = 585;
	static const int BTN_CANCEL_TOP = 600;
	static const int BTN_CANCEL_BOTTOM = 650;
}

void TitleScene::Init()
{
	m_mode = TitleMode::Normal;
	m_requestGoMain = false;
	m_requestGoRanking = false;
	m_nameInput.Reset();
}

void TitleScene::Update()
{
	UiMouse::UpdateFrame();

	if (m_mode == TitleMode::SelectingDifficulty)
	{
		if (UiMouse::TryClick(BTN_LEFT, BTN_PLAY_EASY_TOP, BTN_RIGHT, BTN_PLAY_EASY_BOTTOM))
		{
			GameSession::SetDifficulty(GameDifficulty::Easy);
			m_requestGoMain = true;
		}
		if (UiMouse::TryClick(BTN_LEFT, BTN_PLAY_NORMAL_TOP, BTN_RIGHT, BTN_PLAY_NORMAL_BOTTOM))
		{
			GameSession::SetDifficulty(GameDifficulty::Normal);
			m_requestGoMain = true;
		}
		if (UiMouse::TryClick(BTN_LEFT, BTN_CANCEL_TOP, BTN_RIGHT, BTN_CANCEL_BOTTOM))
		{
			m_mode = TitleMode::Normal;
		}
		return;
	}

	if (UiMouse::TryClick(BTN_LEFT, BTN_PLAY_TOP, BTN_RIGHT, BTN_PLAY_BOTTOM))
	{
		m_mode = TitleMode::SelectingDifficulty;
	}
	if (UiMouse::TryClick(BTN_LEFT, BTN_RANK_TOP, BTN_RIGHT, BTN_RANK_BOTTOM))
	{
		RankingManager::ClearHighlight();
		RankingManager::SetReturnScene(SceneID::Title);
		m_requestGoRanking = true;
	}
	if (UiMouse::TryClick(BTN_LEFT, BTN_QUIT_TOP, BTN_RIGHT, BTN_QUIT_BOTTOM))
	{
		GameSession::RequestQuit();
	}
}

void TitleScene::Draw()
{
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		const int blue = 25 + (y * 50 / SCREEN_HEIGHT);
		DrawLine(0, y, SCREEN_WIDTH, y, GetColor(8, 12, blue));
	}

	for (int i = 0; i < 60; ++i)
	{
		const int x = (i * 173) % SCREEN_WIDTH;
		const int y = (i * 97) % (SCREEN_HEIGHT / 2);
		DrawCircle(x, y, 1 + (i % 2), GetColor(220, 220, 255), TRUE);
	}

	DrawBox(200, 380, SCREEN_WIDTH - 200, SCREEN_HEIGHT - 40, GetColor(40, 28, 22), TRUE);
	DrawCircle(350, 300, 40, GetColor(255, 230, 150), TRUE);
	DrawCircle(640, 280, 35, GetColor(255, 220, 130), TRUE);
	DrawCircle(900, 310, 38, GetColor(255, 235, 160), TRUE);

	SetFontSize(52);
	DrawFormatString(SCREEN_WIDTH / 2 - 420, 120, GetColor(255, 245, 210), STR_TITLE_MAIN);

	SetFontSize(26);
	DrawFormatString(SCREEN_WIDTH / 2 - 300, 200, GetColor(200, 210, 255), STR_TITLE_SUB);

	if (m_mode == TitleMode::SelectingDifficulty)
	{
		UiMouse::DrawButton(BTN_LEFT, BTN_PLAY_EASY_TOP, BTN_RIGHT, BTN_PLAY_EASY_BOTTOM, STR_BTN_PLAY_EASY);
		UiMouse::DrawButton(BTN_LEFT, BTN_PLAY_NORMAL_TOP, BTN_RIGHT, BTN_PLAY_NORMAL_BOTTOM, STR_BTN_PLAY_NORMAL);
		UiMouse::DrawButton(BTN_LEFT, BTN_CANCEL_TOP, BTN_RIGHT, BTN_CANCEL_BOTTOM, STR_BTN_CANCEL);
	}
	else
	{
		UiMouse::DrawButton(BTN_LEFT, BTN_PLAY_TOP, BTN_RIGHT, BTN_PLAY_BOTTOM, STR_BTN_PLAY);
		UiMouse::DrawButton(BTN_LEFT, BTN_RANK_TOP, BTN_RIGHT, BTN_RANK_BOTTOM, STR_BTN_RANKING);
		UiMouse::DrawButton(BTN_LEFT, BTN_QUIT_TOP, BTN_RIGHT, BTN_QUIT_BOTTOM, STR_BTN_QUIT);
	}
}

SceneID TitleScene::GetNextSceneID() const
{
	if (m_requestGoRanking)
	{
		return SceneID::Ranking;
	}
	if (m_requestGoMain)
	{
		return SceneID::Main;
	}
	return SceneID::None;
}
