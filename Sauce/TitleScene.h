#pragma once

#include "NameInput.h"
#include "SceneBase.h"

// タイトル画面の表示モード
enum class TitleMode
{
	Normal,               // 通常のタイトルメニュー
	EnteringName,         // 名前入力中（現在は未使用）
	SelectingDifficulty   // 難易度選択パネルを表示中
};

class TitleScene : public SceneBase
{
public:
	void Init() /*override*/;
	void Update() override;
	void Draw() override;
	SceneID GetNextSceneID() const /*override*/;

private:
	TitleMode m_mode;
	bool      m_requestGoMain;
	bool      m_requestGoRanking;
	bool      m_requestGoSetting;
	NameInput m_nameInput;
};