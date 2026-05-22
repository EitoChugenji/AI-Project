#pragma once

#include "NameInput.h"
#include "SceneBase.h"

class ResultScene : public SceneBase
{
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	SceneID GetNextSceneID() const override;

private:
	bool m_requestGoRanking;
	bool m_requestGoTitle;
	NameInput m_nameInput;
};
