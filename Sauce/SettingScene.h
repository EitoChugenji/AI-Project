#pragma once
#include "SceneBase.h"

// =============================================================================
// SettingScene.h
// マウス感度などの設定を行うシーン
// =============================================================================

class SettingScene : public SceneBase
{
public:
	SettingScene();
	~SettingScene() override; // 環境によっては override を外してください

	void Init() override;
	void Update() override;
	void Draw() override;
	SceneID GetNextSceneID() const override;

private:
	float m_mouseSensitivity;
	bool m_isDraggingSlider;
	bool m_requestGoTitle;
};