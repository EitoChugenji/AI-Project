#pragma once

#include "SceneBase.h"

// シーンの生成・保持・切り替えを一元管理するクラス
// 毎フレーム Update/Draw を呼び出し、シーン側が返す遷移先IDで自動切替する
class SceneManager : public SceneBase
{
public:
	SceneManager();
	~SceneManager();

	// ゲーム起動時に1回だけ呼ぶ。ランキング読込後、初期シーンを生成する
	// 入力: 最初に表示するシーンID
	// 出力: なし
	// 副作用: RankingManager::Load、GameSession::LoadConfig を実行する
	void SetFirstScene(SceneID firstSceneID);

	// 毎フレーム呼ぶ。現在シーンのUpdate後、遷移先があれば切り替えを行う
	// 副作用: マウス感度スケーリング処理、シーン切り替え
	void Update();

	// 毎フレーム呼ぶ。現在シーンのDrawを委譲する
	void Draw();

private:
	// 現在アクティブなシーンへのポインタ
	SceneBase* m_currentScene;

	// シーンIDからシーンオブジェクトを生成して返す（新規シーン追加時はここに case を追加する）
	// 入力: 生成したいシーンID
	// 出力: new で生成した SceneBase* （未知IDの場合は TitleScene にフォールバック）
	SceneBase* CreateScene(SceneID sceneID);

	// 現在シーンを破棄して新しいシーンに切り替える
	// 副作用: m_currentScene の delete と Init 呼び出し
	void ChangeScene(SceneID nextSceneID);
};