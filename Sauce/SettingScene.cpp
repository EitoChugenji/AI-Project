#include "SettingScene.h"
#include "DxLib.h"
#include "GameConfig.h"
#include "GameSession.h"
#include "UiMouse.h"

// スライダーのレイアウト定数（SettingScene.cpp 内の両スライダーで共有）
namespace
{
	const int SLIDER_X_START = 440;
	const int SLIDER_X_END   = 840;
	const int SLIDER_WIDTH   = SLIDER_X_END - SLIDER_X_START;

	// 感度スライダーの数値範囲
	const int SENS_SLIDER_Y = 310;
	const float SENS_MIN    = 0.1f;
	const float SENS_MAX    = 3.0f;

	// カーソルサイズスライダーの数値範囲
	const int CUR_SLIDER_Y  = 430;
	const float CUR_MIN     = 4.0f;
	const float CUR_MAX     = 60.0f;
}

SettingScene::SettingScene()
	: m_mouseSensitivity(1.0f)
	, m_isDraggingSlider(false)
	, m_cursorRadius(8.0f)
	, m_isDraggingCursorSlider(false)
	, m_requestGoTitle(false)
{

}

SettingScene::~SettingScene()
{

}

void SettingScene::Init()
{
	m_mouseSensitivity      = GameSession::GetMouseSensitivity();
	m_cursorRadius          = GameSession::GetCursorRadius();
	m_isDraggingSlider      = false;
	m_isDraggingCursorSlider = false;
	m_requestGoTitle        = false;
}

void SettingScene::Update()
{
	UiMouse::UpdateFrame();

	int mx = 0, my = 0;
	GetMousePoint(&mx, &my);
	const int mouseInput  = GetMouseInput();
	const bool leftPressed = (mouseInput & MOUSE_INPUT_LEFT) != 0;

	// 感度スライダー
	if (leftPressed)
	{
		if (!m_isDraggingSlider)
		{
			if (mx >= SLIDER_X_START - 10 && mx <= SLIDER_X_END + 10 &&
				my >= SENS_SLIDER_Y - 20  && my <= SENS_SLIDER_Y + 20)
			{
				m_isDraggingSlider = true;
			}
		}

		if (m_isDraggingSlider)
		{
			float ratio = static_cast<float>(mx - SLIDER_X_START) / static_cast<float>(SLIDER_WIDTH);
			if (ratio < 0.0f) ratio = 0.0f;
			if (ratio > 1.0f) ratio = 1.0f;
			m_mouseSensitivity = SENS_MIN + ratio * (SENS_MAX - SENS_MIN);
			GameSession::SetMouseSensitivity(m_mouseSensitivity);
		}
	}
	// マウスボタンが離れたらドラッグ状態を解除する
	else
	{
		m_isDraggingSlider = false;
	}

	// 感度 ± ボタン
	if (UiMouse::TryClick(380, SENS_SLIDER_Y - 20, 420, SENS_SLIDER_Y + 20))
	{
		m_mouseSensitivity -= 0.01f;
		if (m_mouseSensitivity < SENS_MIN) m_mouseSensitivity = SENS_MIN;
		GameSession::SetMouseSensitivity(m_mouseSensitivity);
	}

	if (UiMouse::TryClick(860, SENS_SLIDER_Y - 20, 900, SENS_SLIDER_Y + 20))
	{
		m_mouseSensitivity += 0.05f;
		if (m_mouseSensitivity > SENS_MAX) m_mouseSensitivity = SENS_MAX;
		GameSession::SetMouseSensitivity(m_mouseSensitivity);
	}

	// カーソルサイズスライダー
	if (leftPressed)
	{
		if (!m_isDraggingCursorSlider)
		{
			if (mx >= SLIDER_X_START - 10 && mx <= SLIDER_X_END + 10 &&
				my >= CUR_SLIDER_Y - 20   && my <= CUR_SLIDER_Y + 20)
			{
				m_isDraggingCursorSlider = true;
			}
		}

		if (m_isDraggingCursorSlider)
		{
			float ratio = static_cast<float>(mx - SLIDER_X_START) / static_cast<float>(SLIDER_WIDTH);
			if (ratio < 0.0f) ratio = 0.0f;
			if (ratio > 1.0f) ratio = 1.0f;
			m_cursorRadius = CUR_MIN + ratio * (CUR_MAX - CUR_MIN);
			GameSession::SetCursorRadius(m_cursorRadius);
		}
	}
	// マウスボタンが離れたらドラッグ状態を解除する
	else
	{
		m_isDraggingCursorSlider = false;
	}

	// カーソルサイズ ± ボタン
	if (UiMouse::TryClick(380, CUR_SLIDER_Y - 20, 420, CUR_SLIDER_Y + 20))
	{
		m_cursorRadius -= 2.0f;
		if (m_cursorRadius < CUR_MIN) m_cursorRadius = CUR_MIN;
		GameSession::SetCursorRadius(m_cursorRadius);
	}

	if (UiMouse::TryClick(860, CUR_SLIDER_Y - 20, 900, CUR_SLIDER_Y + 20))
	{
		m_cursorRadius += 2.0f;
		if (m_cursorRadius > CUR_MAX) m_cursorRadius = CUR_MAX;
		GameSession::SetCursorRadius(m_cursorRadius);
	}

	//// デバッグモード切り替え
	//if (UiMouse::TryClick(390, 480, 500, 510))
	//{
	//	GameSession::SetDebugModeEnabled(!GameSession::GetDebugModeEnabled());
	//}

	//if (GameSession::GetDebugModeEnabled())
	//{
	//	// 中央オートクリック
	//	if (UiMouse::TryClick(520, 480, 730, 510))
	//	{
	//		GameSession::SetDebugAutoClick(!GameSession::GetDebugAutoClick());
	//	}
	// 
	//	// コンボ維持
	//	if (UiMouse::TryClick(750, 480, 960, 510))
	//	{
	//		GameSession::SetDebugNoComboBreak(!GameSession::GetDebugNoComboBreak());
	//	}
	// 
	//	// 時間停止
	//	if (UiMouse::TryClick(390, 525, 600, 555))
	//	{
	//		GameSession::SetDebugInfiniteTime(!GameSession::GetDebugInfiniteTime());
	//	}
	// 
	//	// 罠無効
	//	if (UiMouse::TryClick(620, 525, 830, 555))
	//	{
	//		GameSession::SetDebugNoTrapPenalty(!GameSession::GetDebugNoTrapPenalty());
	//	}
	//}

	// 設定を保存してからタイトルに戻る（SaveConfig で .ini 等に永続化）
	if (UiMouse::TryClick(440, 590, 840, 640))
	{
		GameSession::SaveConfig();
		m_requestGoTitle = true;
	}
}

void SettingScene::Draw()
{
	// 背景
	DrawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GetColor(15, 10, 8), TRUE);
	for (int i = 0; i < 40; ++i)
	{
		int x = (i * 37) % SCREEN_WIDTH;
		int y = (i * 23) % SCREEN_HEIGHT;
		DrawCircle(x, y, 1, GetColor(100, 100, 130), TRUE);
	}

	// パネル
	DrawBox(200, 80, SCREEN_WIDTH - 200, SCREEN_HEIGHT - 60, GetColor(40, 28, 22), TRUE);
	DrawBox(200, 80, SCREEN_WIDTH - 200, SCREEN_HEIGHT - 60, GetColor(180, 140, 100), FALSE);

	// タイトル
	SetFontSize(36);
	DrawFormatString(SCREEN_WIDTH / 2 - 72, 100, GetColor(255, 245, 210), L"設定画面");

	// ==============================
	// 感度スライダー
	// ==============================
	SetFontSize(24);
	DrawFormatString(SCREEN_WIDTH / 2 - 160, 200, GetColor(230, 230, 230), L"マウスの操作速度を調整します。");
	DrawFormatString(SCREEN_WIDTH / 2 - 80,  240, GetColor(255, 255, 255), L"マウス感度: %.2f", m_mouseSensitivity);

	{
		int knobX = SLIDER_X_START + static_cast<int>(SLIDER_WIDTH * ((m_mouseSensitivity - SENS_MIN) / (SENS_MAX - SENS_MIN)));
		DrawBox(SLIDER_X_START, SENS_SLIDER_Y - 4, SLIDER_X_END, SENS_SLIDER_Y + 4, GetColor(80, 70, 65), TRUE);
		DrawBox(SLIDER_X_START, SENS_SLIDER_Y - 4, knobX,        SENS_SLIDER_Y + 4, GetColor(135, 206, 250), TRUE);
		DrawCircle(knobX, SENS_SLIDER_Y, 12, GetColor(255, 220, 100), TRUE);
		DrawCircle(knobX, SENS_SLIDER_Y, 12, GetColor(255, 255, 255), FALSE);
		UiMouse::DrawButton(380, SENS_SLIDER_Y - 20, 420, SENS_SLIDER_Y + 20, L"-");
		UiMouse::DrawButton(860, SENS_SLIDER_Y - 20, 900, SENS_SLIDER_Y + 20, L"+");
	}

	// ==============================
	// カーソルサイズスライダー
	// ==============================
	SetFontSize(24);
	DrawFormatString(SCREEN_WIDTH / 2 - 180, 355, GetColor(230, 230, 230), L"カーソルサイズ（当たり判定）を変更します。");
	DrawFormatString(SCREEN_WIDTH / 2 - 80,  392, GetColor(255, 255, 255), L"カーソル半径: %.0f px", m_cursorRadius);

	{
		int knobX = SLIDER_X_START + static_cast<int>(SLIDER_WIDTH * ((m_cursorRadius - CUR_MIN) / (CUR_MAX - CUR_MIN)));
		DrawBox(SLIDER_X_START, CUR_SLIDER_Y - 4, SLIDER_X_END, CUR_SLIDER_Y + 4, GetColor(80, 70, 65), TRUE);
		DrawBox(SLIDER_X_START, CUR_SLIDER_Y - 4, knobX,        CUR_SLIDER_Y + 4, GetColor(150, 255, 180), TRUE);
		DrawCircle(knobX, CUR_SLIDER_Y, 12, GetColor(120, 255, 160), TRUE);
		DrawCircle(knobX, CUR_SLIDER_Y, 12, GetColor(255, 255, 255), FALSE);
		UiMouse::DrawButton(380, CUR_SLIDER_Y - 20, 420, CUR_SLIDER_Y + 20, L"-");
		UiMouse::DrawButton(860, CUR_SLIDER_Y - 20, 900, CUR_SLIDER_Y + 20, L"+");
	}

	// カーソルサイズを変更しながらプレビューで即確認できるよう右端に表示する
	{
		const int previewX = 1050;
		const int previewY = CUR_SLIDER_Y;
		SetFontSize(16);
		DrawFormatString(previewX - 30, previewY - 50, GetColor(180, 180, 180), L"プレビュー");

		// 当たり判定円
		const int pr = static_cast<int>(m_cursorRadius);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 55);
		DrawCircle(previewX, previewY, pr, GetColor(100, 200, 255), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawCircle(previewX, previewY, pr, GetColor(80, 180, 255), FALSE);

		// 中心
		DrawCircle(previewX, previewY, 5, GetColor(255, 255, 255), TRUE);
		DrawCircle(previewX, previewY, 5, GetColor(0, 0, 0), FALSE);

		// 十字線
		DrawLine(previewX - 12, previewY, previewX - 6, previewY, GetColor(255, 255, 255));
		DrawLine(previewX + 6,  previewY, previewX + 12, previewY, GetColor(255, 255, 255));
		DrawLine(previewX, previewY - 12, previewX, previewY - 6, GetColor(255, 255, 255));
		DrawLine(previewX, previewY + 6,  previewX, previewY + 12, GetColor(255, 255, 255));
	}

	// ==============================
	// デバッグモードパネル
	// ==============================
	//{
	//	// パネル背景（半透明赤）
	//	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	//	DrawRoundRect(220, 470, 1060, 575, 8, 8, GetColor(35, 15, 15), TRUE);
	//	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	//	DrawRoundRect(220, 470, 1060, 575, 8, 8, GetColor(255, 80, 80), FALSE);

	//	SetFontSize(20);
	//	DrawFormatString(240, 485, GetColor(255, 200, 200), L"デバッグ機能:");

	//	// メインスイッチ
	//	UiMouse::DrawButton(390, 480, 500, 510, GameSession::GetDebugModeEnabled() ? L"有効" : L"無効");

	//	if (GameSession::GetDebugModeEnabled())
	//	{
	//		// 中央オートクリック
	//		UiMouse::DrawButton(520, 480, 730, 510, GameSession::GetDebugAutoClick() ? L"オート:ON" : L"オート:OFF");
	//		// コンボ維持
	//		UiMouse::DrawButton(750, 480, 960, 510, GameSession::GetDebugNoComboBreak() ? L"コンボ維持:ON" : L"コンボ維持:OFF");
	//		// 時間停止
	//		UiMouse::DrawButton(390, 525, 600, 555, GameSession::GetDebugInfiniteTime() ? L"時間停止:ON" : L"時間停止:OFF");
	//		// 罠無効
	//		UiMouse::DrawButton(620, 525, 830, 555, GameSession::GetDebugNoTrapPenalty() ? L"罠無効:ON" : L"罠無効:OFF");
	//	}
	// 
	//	else
	//	{
	//		SetFontSize(18);
	//		DrawFormatString(520, 486, GetColor(150, 120, 120), L"※デバッグを「有効」にすると機能切り替えが可能になります。");
	//	}
	//}

	// 保存して戻るボタン
	UiMouse::DrawButton(440, 590, 840, 640, L"保存してタイトルに戻る");

	// カスタムカーソル（設定画面でも表示・当たり判定円は非表示）
	UiMouse::DrawCursor(m_cursorRadius, false);
}

SceneID SettingScene::GetNextSceneID() const
{
	if (m_requestGoTitle)
	{
		return SceneID::Title;
	}

	return SceneID::None;
}
