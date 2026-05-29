#pragma once
#include "SceneBase.h"

// マウス感度とカーソルサイズを変更できる設定シーン
// スライダーとボタンで値を調整し、保存してタイトルに戻る
class SettingScene : public SceneBase
{
public:
	SettingScene();
	~SettingScene() override;

	void Init() /*override*/;
	void Update() override;
	void Draw() override;
	SceneID GetNextSceneID() const /*override*/;

private:
	float m_mouseSensitivity;
	bool  m_isDraggingSlider;

	float m_cursorRadius;
	bool  m_isDraggingCursorSlider;

	bool  m_requestGoTitle;
};