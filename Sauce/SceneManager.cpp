#include "SceneManager.h"

#include "TitleScene.h"
#include "MainScene.h"
#include "ResultScene.h"
#include "RankingScene.h"
#include "RankingManager.h"

// =============================================================================
// SceneManager.cpp
// シーンの生成・破棄・切り替えの実装
// =============================================================================

SceneManager::SceneManager()
	: m_currentScene(nullptr)
{
}

SceneManager::~SceneManager()
{
	// ゲーム終了時に、最後のシーンのメモリを解放
	if (m_currentScene != nullptr)
	{
		delete m_currentScene;
		m_currentScene = nullptr;
	}
}

void SceneManager::SetFirstScene(SceneID firstSceneID)
{
	RankingManager::Load();
	ChangeScene(firstSceneID);
}

void SceneManager::Update()
{
	// 現在のシーンが無ければ何もしない（安全のため）
	if (m_currentScene == nullptr)
	{
		return;
	}

	// 1. 現在シーンの更新処理
	m_currentScene->Update();

	// 2. 遷移先が指定されていればシーン切り替え
	SceneID nextID = m_currentScene->GetNextSceneID();
	if (nextID != SceneID::None)
	{
		ChangeScene(nextID);
	}
}

void SceneManager::Draw()
{
	if (m_currentScene == nullptr)
	{
		return;
	}

	m_currentScene->Draw();
}

SceneBase* SceneManager::CreateScene(SceneID sceneID)
{
	// シーンIDに応じて、対応するクラスのインスタンスを new で生成
	// 【追加シーンを作るとき】
	//   1. 新しい XxxScene.h / .cpp を作る
	//   2. ここに #include と case を追加する
	switch (sceneID)
	{
	case SceneID::Title:
		return new TitleScene();

	case SceneID::Main:
		return new MainScene();

	case SceneID::Result:
		return new ResultScene();

	case SceneID::Ranking:
		return new RankingScene();

	default:
		// 想定外のIDのときはタイトルにフォールバック
		return new TitleScene();
	}
}

void SceneManager::ChangeScene(SceneID nextSceneID)
{
	// 古いシーンを破棄
	if (m_currentScene != nullptr)
	{
		delete m_currentScene;
		m_currentScene = nullptr;
	}

	// 新しいシーンを作成
	m_currentScene = CreateScene(nextSceneID);

	// 新シーンの初期化（画像読み込みなど）
	if (m_currentScene != nullptr)
	{
		m_currentScene->Init();
	}
}
