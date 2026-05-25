#include "SettingScene.h"
#include "DxLib.h"
#include "GameConfig.h"
#include "GameSession.h"
#include "UiMouse.h"

SettingScene::SettingScene()
	: m_mouseSensitivity(1.0f)
	, m_isDraggingSlider(false)
	, m_requestGoTitle(false)
{
}

SettingScene::~SettingScene()
{
}

void SettingScene::Init()
{
	m_mouseSensitivity = GameSession::GetMouseSensitivity();
	m_isDraggingSlider = false;
	m_requestGoTitle = false;
}

void SettingScene::Update()
{
	UiMouse::UpdateFrame();

	const int sliderXStart = 440;
	const int sliderXEnd = 840;
	const int sliderY = 350;
	const int sliderWidth = sliderXEnd - sliderXStart;

	// Mouse inputs
	int mx = 0, my = 0;
	GetMousePoint(&mx, &my);
	const int mouseInput = GetMouseInput();
	const bool leftPressed = (mouseInput & MOUSE_INPUT_LEFT) != 0;

	float minSens = 0.1f;
	float maxSens = 3.0f;

	// Dragging logic
	if (leftPressed)
	{
		if (!m_isDraggingSlider)
		{
			// Check if click starts inside slider bounding box (with padding)
			if (mx >= sliderXStart - 10 && mx <= sliderXEnd + 10 &&
				my >= sliderY - 20 && my <= sliderY + 20)
			{
				m_isDraggingSlider = true;
			}
		}

		if (m_isDraggingSlider)
		{
			float ratio = static_cast<float>(mx - sliderXStart) / static_cast<float>(sliderWidth);
			if (ratio < 0.0f) ratio = 0.0f;
			if (ratio > 1.0f) ratio = 1.0f;
			m_mouseSensitivity = minSens + ratio * (maxSens - minSens);
			GameSession::SetMouseSensitivity(m_mouseSensitivity);
		}
	}
	else
	{
		m_isDraggingSlider = false;
	}

	// Fine-tune decrease (-) button
	if (UiMouse::TryClick(380, sliderY - 20, 420, sliderY + 20))
	{
		m_mouseSensitivity -= 0.05f;
		if (m_mouseSensitivity < minSens) m_mouseSensitivity = minSens;
		GameSession::SetMouseSensitivity(m_mouseSensitivity);
	}

	// Fine-tune increase (+) button
	if (UiMouse::TryClick(860, sliderY - 20, 900, sliderY + 20))
	{
		m_mouseSensitivity += 0.05f;
		if (m_mouseSensitivity > maxSens) m_mouseSensitivity = maxSens;
		GameSession::SetMouseSensitivity(m_mouseSensitivity);
	}

	// Save and return button
	if (UiMouse::TryClick(440, 500, 840, 550))
	{
		GameSession::SaveConfig();
		m_requestGoTitle = true;
	}
}

void SettingScene::Draw()
{
	// Dark space background
	DrawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GetColor(15, 10, 8), TRUE);

	// Gentle background star particles
	for (int i = 0; i < 40; ++i)
	{
		int x = (i * 37) % SCREEN_WIDTH;
		int y = (i * 23) % SCREEN_HEIGHT;
		DrawCircle(x, y, 1, GetColor(100, 100, 130), TRUE);
	}

	// Center settings panel
	DrawBox(200, 200, SCREEN_WIDTH - 200, SCREEN_HEIGHT - 100, GetColor(40, 28, 22), TRUE);
	DrawBox(200, 200, SCREEN_WIDTH - 200, SCREEN_HEIGHT - 100, GetColor(180, 140, 100), FALSE);

	// Header
	DrawFormatString(SCREEN_WIDTH / 2 - 64, 120, GetColor(255, 245, 210), L"\u8A2D\u5B9A\u753B\u9762");

	// Instructions and Value
	DrawFormatString(SCREEN_WIDTH / 2 - 150, 240, GetColor(230, 230, 230), L"\u30DE\u30A6\u30B9\u306E\u64CD\u4F5C\u901F\u5EA6\u3092\u8ABF\u6574\u3057\u307E\u3059\u3002");
	DrawFormatString(SCREEN_WIDTH / 2 - 80, 280, GetColor(255, 255, 255), L"\u30DE\u30A6\u30B9\u611F\u5EA6: %.2f", m_mouseSensitivity);

	const int sliderXStart = 440;
	const int sliderXEnd = 840;
	const int sliderY = 350;
	const int sliderWidth = sliderXEnd - sliderXStart;

	float minSens = 0.1f;
	float maxSens = 3.0f;
	int knobX = sliderXStart + static_cast<int>(sliderWidth * ((m_mouseSensitivity - minSens) / (maxSens - minSens)));

	// Slider Track (inactive and active parts)
	DrawBox(sliderXStart, sliderY - 4, sliderXEnd, sliderY + 4, GetColor(80, 70, 65), TRUE);
	DrawBox(sliderXStart, sliderY - 4, knobX, sliderY + 4, GetColor(135, 206, 250), TRUE);

	// Slider Knob
	DrawCircle(knobX, sliderY, 12, GetColor(255, 220, 100), TRUE);
	DrawCircle(knobX, sliderY, 12, GetColor(255, 255, 255), FALSE);

	// Fine-tune buttons (- and +)
	UiMouse::DrawButton(380, sliderY - 20, 420, sliderY + 20, L"-");
	UiMouse::DrawButton(860, sliderY - 20, 900, sliderY + 20, L"+");

	// Return button
	UiMouse::DrawButton(440, 500, 840, 550, L"\u4FDD\u5B58\u3057\u3066\u30BF\u30A4\u30C8\u30EB\u306B\u623B\u308B");
}

SceneID SettingScene::GetNextSceneID() const
{
	if (m_requestGoTitle)
	{
		return SceneID::Title;
	}
	return SceneID::None;
}