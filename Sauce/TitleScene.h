#pragma once

#include "NameInput.h"
#include "SceneBase.h"

enum class TitleMode
{
	Normal,
	EnteringName,
	SelectingDifficulty
};

class TitleScene : public SceneBase
{
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	SceneID GetNextSceneID() const override;

private:
	TitleMode m_mode;
	bool m_requestGoMain;
	bool m_requestGoRanking;
	bool m_requestGoSetting;
	NameInput m_nameInput;
};
