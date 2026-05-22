#pragma once

#include "SceneBase.h"

// =============================================================================
// SceneManager.h
// 現在表示中のシーンを1つだけ保持し、
// 「更新」「描画」「シーン切り替え」をまとめて行うクラスです。
//
// 【シーン遷移の流れ（このゲーム）】
//   タイトル → メイン → リザルト
//   （リザルトからタイトルに戻る処理も各シーン側で指定します）
// =============================================================================

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	// 最初に表示するシーンをセット（WinMain から1回呼ぶ）
	void SetFirstScene(SceneID firstSceneID);

	// 毎フレーム呼ぶ：現在シーンの Update → 必要ならシーン切り替え
	void Update();

	// 毎フレーム呼ぶ：現在シーンの Draw
	void Draw();

private:
	// 今動いているシーンへのポインタ
	SceneBase* m_currentScene;

	// 指定IDのシーンオブジェクトを新しく作る（内部用）
	SceneBase* CreateScene(SceneID sceneID);

	// 今のシーンを破棄して、新しいシーンに切り替える（内部用）
	void ChangeScene(SceneID nextSceneID);
};
