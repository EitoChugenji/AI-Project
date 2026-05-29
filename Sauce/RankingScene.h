#pragma once

#include "SceneBase.h"

// 難易度ごとのスコアランキングを表示するシーン
// タブ切替で Easy/Normal/Hell の各ランキングを参照できる
class RankingScene : public SceneBase
{
public:
	void Init() /*override*/;
	void Update() override;
	void Draw() override;
	SceneID GetNextSceneID() const /*override*/;

private:
	// 「戻る」ボタンが押されたとき true になり、呼び出し元シーンに戻る
	bool m_requestReturn;
};