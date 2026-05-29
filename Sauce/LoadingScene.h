#pragma once

#include "SceneBase.h"

class LoadingScene : public SceneBase
{
public:
	void Init() /*override*/;
	void Update() override;
	void Draw() override;
	SceneID GetNextSceneID() const /*override*/;

	static void SetTargetScene(SceneID target);

private:
	static SceneID s_targetScene;
	int m_frameCounter;
	int m_maxFrames;
	int m_tipIndex;
};
