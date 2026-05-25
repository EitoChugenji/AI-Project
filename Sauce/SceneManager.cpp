#include "SceneManager.h"

#include "TitleScene.h"
#include "MainScene.h"
#include "ResultScene.h"
#include "RankingScene.h"
#include "RankingManager.h"
#include "SettingScene.h"
#include "GameConfig.h"
#include "GameSession.h"
#include "DxLib.h"

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
	GameSession::LoadConfig();
	ChangeScene(firstSceneID);
}

void SceneManager::Update()
{
	// Global mouse sensitivity scaling
	{
		int rawX = 0, rawY = 0;
		GetMousePoint(&rawX, &rawY);

		static int s_prevMouseX = 0;
		static int s_prevMouseY = 0;
		static float s_mouseXAcc = 0.0f;
		static float s_mouseYAcc = 0.0f;
		static bool s_mouseInitialized = false;
		// SetMousePoint を呼んだ直後のフレームはデルタをスキップする
		static bool s_skipNextDelta = false;

		if (!s_mouseInitialized)
		{
			s_prevMouseX = rawX;
			s_prevMouseY = rawY;
			s_mouseXAcc = static_cast<float>(rawX);
			s_mouseYAcc = static_cast<float>(rawY);
			s_mouseInitialized = true;
			s_skipNextDelta = false;
		}
		else
		{
			float sensitivity = GameSession::GetMouseSensitivity();
			if (sensitivity != 1.0f)
			{
				if (s_skipNextDelta)
				{
					// SetMousePoint 直後フレーム: OSが返す座標を現在位置として記録するだけ
					s_prevMouseX = rawX;
					s_prevMouseY = rawY;
					// 蓄積値もrawに合わせてリセット（丸め誤差を吸収）
					s_mouseXAcc = static_cast<float>(rawX);
					s_mouseYAcc = static_cast<float>(rawY);
					s_skipNextDelta = false;
				}
				else
				{
					int dx = rawX - s_prevMouseX;
					int dy = rawY - s_prevMouseY;

					if (dx != 0 || dy != 0)
					{
						s_mouseXAcc += dx * sensitivity;
						s_mouseYAcc += dy * sensitivity;

						// Clamp to screen bounds
						if (s_mouseXAcc < 0.0f) s_mouseXAcc = 0.0f;
						if (s_mouseXAcc > SCREEN_WIDTH) s_mouseXAcc = static_cast<float>(SCREEN_WIDTH);
						if (s_mouseYAcc < 0.0f) s_mouseYAcc = 0.0f;
						if (s_mouseYAcc > SCREEN_HEIGHT) s_mouseYAcc = static_cast<float>(SCREEN_HEIGHT);

						int targetX = static_cast<int>(s_mouseXAcc);
						int targetY = static_cast<int>(s_mouseYAcc);

						SetMousePoint(targetX, targetY);

						s_prevMouseX = targetX;
						s_prevMouseY = targetY;
						s_skipNextDelta = true; // 次フレームはスキップ
					}
					else
					{
						// 動いていないときは prev を追従させる
						s_prevMouseX = rawX;
						s_prevMouseY = rawY;
					}
				}
			}
			else
			{
				s_prevMouseX = rawX;
				s_prevMouseY = rawY;
				s_mouseXAcc = static_cast<float>(rawX);
				s_mouseYAcc = static_cast<float>(rawY);
				s_skipNextDelta = false;
			}
		}
	}

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

	case SceneID::Setting:
		return new SettingScene();

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
