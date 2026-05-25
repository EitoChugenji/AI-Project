#pragma once

#include "Collision.h"
#include "GameConfig.h"
#include "SceneBase.h"

// オブジェクトの種類
enum class EntityKind
{
	Animal,     // 円 … クリックで加点
	Item,       // 円 … クリックで少し加点
	Obstacle,   // 矩形 … クリックでペナルティ
};

enum class CrimeType
{
	Hack,
	Bomb,
	Ritual,
	Snack,
	Trap,
};

struct GameEntity
{
	bool active;
	EntityKind kind;
	CrimeType crime;

	float x;
	float y;
	float vx;
	float vy;

	float radius;
	bool usesRectCollider;
	float halfWidth;
	float halfHeight;

	int bodyColor;
	int accentColor;
};

struct PopupText
{
	bool active;
	float x;
	float y;
	float vy;
	int life;
	int color;
	CrimeType crime;
};

struct Star
{
	int x;
	int y;
	int brightness;
};

class MainScene : public SceneBase
{
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	SceneID GetNextSceneID() const override;

private:
	void ResetGame();
	void UpdateTimer();
	void UpdateSpawning();
	void UpdateEntities();
	void UpdatePopups();
	void UpdateInput();
	void UpdateIdleTimer();
	void UpdateFever();

	bool TrySpawnEntity();
	int CountActiveEntities() const;
	float GetDifficultyProgress() const;
	float GetCurrentFallSpeed() const;
	int GetSpawnIntervalFrames() const;
	int GetMaxConcurrentObjects() const;

	void SpawnPopup(float x, float y, CrimeType crime);
	bool IsEntityOffScreen(const GameEntity& entity) const;
	void OnEntityFallOffScreen(GameEntity& entity);
	void EndGame(bool isGameOver);

	bool IsClickOnEntity(const GameEntity& entity, float mouseX, float mouseY) const;

	void DrawBackground();
	void DrawPlayArea();
	void DrawEntities();
	void DrawPopups();
	void DrawUI();

	float RandomRange(float minValue, float maxValue);

	bool m_requestGoResult;
	int m_score;
	int m_combo;
	int m_maxCombo;
	int m_comboTimer;

	int m_timeLeftSec;
	int m_secondCounter;

	int m_spawnTimer;
	int m_feverTimer;
	int m_idleTimerFrames;
	int m_hitFlashTimer;

	int m_prevMouseInput;

	GameEntity m_entities[MAX_FALLING_ENTITIES];
	PopupText m_popups[MAX_POPUP_TEXTS];
	Star m_stars[MAX_STARS];
};