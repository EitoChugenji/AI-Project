#pragma once

#include "SceneBase.h"

class RankingScene : public SceneBase
{
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	SceneID GetNextSceneID() const override;

private:
	bool m_requestReturn;
};
