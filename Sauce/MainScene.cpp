#include "MainScene.h"

#include "Collision.h"
#include "GameConfig.h"
#include "GameStrings.h"
#include "GameSession.h"
#include "UiMouse.h"
#include "DxLib.h"

#include <cmath>

namespace
{
	const wchar_t* GetCrimeString(CrimeType crime)
	{
		switch (crime)
		{
		case CrimeType::Hack:   return STR_CRIME_HACK;
		case CrimeType::Bomb:   return STR_CRIME_BOMB;
		case CrimeType::Ritual: return STR_CRIME_RITUAL;
		case CrimeType::Snack:  return STR_CRIME_SNACK;
		case CrimeType::Trap:   return STR_CRIME_TRAP;
		default:                return STR_CRIME_HACK;
		}
	}
}

void MainScene::Init()
{
	GameSession::Reset();
	ResetGame();
}

void MainScene::ResetGame()
{
	m_requestGoResult = false;
	m_score = 0;
	m_combo = 0;
	m_maxCombo = 0;
	m_comboTimer = 0;
	m_timeLeftSec = GAME_DURATION_SEC;
	m_secondCounter = 0;
	m_spawnTimer = 0;
	m_prevMouseInput = 0;
	m_feverTimer = 0;
	m_idleTimerFrames = 0;
	m_hitFlashTimer = 0;

	for (int i = 0; i < MAX_FALLING_ENTITIES; ++i) m_entities[i].active = false;
	for (int i = 0; i < MAX_POPUP_TEXTS; ++i) m_popups[i].active = false;
	for (int i = 0; i < MAX_CLICK_PARTICLES; ++i) m_particles[i].active = false;

	for (int i = 0; i < MAX_STARS; ++i)
	{
		m_stars[i].x = GetRand(SCREEN_WIDTH);
		m_stars[i].y = GetRand(SCREEN_HEIGHT / 2);
		m_stars[i].brightness = 150 + GetRand(106);
	}
}

float MainScene::RandomRange(float minValue, float maxValue)
{
	const int range = static_cast<int>(maxValue - minValue + 1.0f);
	return minValue + static_cast<float>(GetRand(range > 0 ? range : 1));
}

float MainScene::GetDifficultyProgress() const
{
	if (GAME_DURATION_SEC <= 0) return 1.0f;
	const float elapsed = static_cast<float>(GAME_DURATION_SEC - m_timeLeftSec);
	float progress = elapsed / static_cast<float>(GAME_DURATION_SEC);
	return (progress > 1.0f) ? 1.0f : progress;
}

float MainScene::GetCurrentFallSpeed() const
{
	const float progress = GetDifficultyProgress();
	float speed = FALL_SPEED_BASE + (FALL_SPEED_MAX - FALL_SPEED_BASE) * progress;

	if (GameSession::GetDifficulty() == GameDifficulty::Easy) return speed * 0.6f;
	if (GameSession::GetDifficulty() == GameDifficulty::Hell) return speed * 2.5f;
	return speed;
}

int MainScene::GetSpawnIntervalFrames() const
{
	const float progress = GetDifficultyProgress();
	int interval = static_cast<int>(SPAWN_INTERVAL_MAX_FRAMES - (SPAWN_INTERVAL_MAX_FRAMES - SPAWN_INTERVAL_MIN_FRAMES) * progress);

	if (GameSession::GetDifficulty() == GameDifficulty::Easy) return static_cast<int>(interval * 1.5f);
	if (GameSession::GetDifficulty() == GameDifficulty::Hell) return (interval < 6) ? 2 : (int)(interval * 0.3f);
	return interval;
}

int MainScene::GetMaxConcurrentObjects() const
{
	const float progress = GetDifficultyProgress();
	const float count = static_cast<float>(MAX_ON_SCREEN_MIN) + (static_cast<float>(MAX_ON_SCREEN_MAX) - static_cast<float>(MAX_ON_SCREEN_MIN)) * progress;
	return static_cast<int>(count + 0.5f);
}

int MainScene::CountActiveEntities() const
{
	int count = 0;
	for (int i = 0; i < MAX_FALLING_ENTITIES; ++i) if (m_entities[i].active) ++count;
	return count;
}

bool MainScene::TrySpawnEntity()
{
	if (CountActiveEntities() >= GetMaxConcurrentObjects()) return false;

	int slot = -1;
	for (int i = 0; i < MAX_FALLING_ENTITIES; ++i) { if (!m_entities[i].active) { slot = i; break; } }
	if (slot < 0) return false;

	GameEntity& entity = m_entities[slot];
	int spawnPercent = OBSTACLE_SPAWN_PERCENT;
	if (GameSession::GetDifficulty() == GameDifficulty::Easy) spawnPercent /= 2;
	if (GameSession::GetDifficulty() == GameDifficulty::Hell) spawnPercent += 20;

	const bool spawnObstacle = GetRand(100) < spawnPercent;
	const float fallSpeed = GetCurrentFallSpeed();

	entity.active = true;
	entity.vx = RandomRange(-0.6f, 0.6f);
	entity.vy = fallSpeed + RandomRange(-0.3f, 0.5f);

	if (spawnObstacle)
	{
		entity.kind = EntityKind::Obstacle;
		entity.crime = CrimeType::Trap;
		entity.usesRectCollider = true;
		entity.halfWidth = 36.0f + static_cast<float>(GetRand(18));
		entity.halfHeight = 26.0f + static_cast<float>(GetRand(12));
		entity.x = RandomRange(static_cast<float>(ROOM_LEFT) + entity.halfWidth, static_cast<float>(ROOM_RIGHT) - entity.halfWidth);
		entity.y = static_cast<float>(ROOM_TOP) - entity.halfHeight - 8.0f;
		entity.bodyColor = ::GetColor(200, 55, 75);
		entity.accentColor = ::GetColor(130, 25, 40);
	}
	else if (GetRand(100) < 35)
	{
		entity.kind = EntityKind::Item;
		entity.crime = CrimeType::Snack;
		entity.usesRectCollider = false;
		entity.radius = 18.0f + static_cast<float>(GetRand(10));
		entity.x = RandomRange(static_cast<float>(ROOM_LEFT) + entity.radius, static_cast<float>(ROOM_RIGHT) - entity.radius);
		entity.y = static_cast<float>(ROOM_TOP) - entity.radius - 8.0f;
		entity.bodyColor = ::GetColor(255, 220, 120);
		entity.accentColor = ::GetColor(200, 150, 60);
	}
	else
	{
		entity.kind = EntityKind::Animal;
		entity.crime = static_cast<CrimeType>(GetRand(4));
		entity.usesRectCollider = false;
		entity.radius = 30.0f + static_cast<float>(GetRand(12));
		entity.x = RandomRange(static_cast<float>(ROOM_LEFT) + entity.radius, static_cast<float>(ROOM_RIGHT) - entity.radius);
		entity.y = static_cast<float>(ROOM_TOP) - entity.radius - 8.0f;
		entity.bodyColor = ::GetColor(240, 210 + GetRand(40), 180 + GetRand(50));
		entity.accentColor = ::GetColor(80, 60, 50);
	}
	return true;
}

void MainScene::UpdateSpawning()
{
	if (m_spawnTimer > 0) { --m_spawnTimer; return; }
	if (TrySpawnEntity()) m_spawnTimer = GetSpawnIntervalFrames();
	else m_spawnTimer = 8;
}

void MainScene::Update()
{
	if (m_timeLeftSec <= 0) { EndGame(false); return; }
	UpdateTimer();
	UpdateSpawning();
	UpdateEntities();
	UpdatePopups();
	UpdateParticles();
	UpdateInput();
	UpdateIdleTimer();
	UpdateFever();

	if (m_comboTimer > 0) { if (--m_comboTimer <= 0) m_combo = 0; }
	if (m_hitFlashTimer > 0) --m_hitFlashTimer;
	if (m_feverTimer > 0) --m_feverTimer;
}

void MainScene::EndGame(bool isGameOver)
{
	GameSession::SetLastResult(m_score, m_maxCombo, isGameOver);
	m_requestGoResult = true;
}

void MainScene::UpdateTimer()
{
	if (++m_secondCounter >= TARGET_FPS) { m_secondCounter = 0; --m_timeLeftSec; }
}

bool MainScene::IsEntityOffScreen(const GameEntity& entity) const
{
	float bottom = entity.usesRectCollider ? entity.y + entity.halfHeight : entity.y + entity.radius;
	return bottom > static_cast<float>(ROOM_BOTTOM) + 40.0f;
}

void MainScene::OnEntityFallOffScreen(GameEntity& entity)
{
	if (entity.kind == EntityKind::Obstacle) return;
	m_score = (m_score < (entity.kind == EntityKind::Animal ? MISS_ANIMAL_PENALTY_SCORE : MISS_ITEM_PENALTY_SCORE)) ? 0 : (m_score - (entity.kind == EntityKind::Animal ? MISS_ANIMAL_PENALTY_SCORE : MISS_ITEM_PENALTY_SCORE));
	m_combo = 0; m_comboTimer = 0;
}

void MainScene::UpdateEntities()
{
	const float fallSpeed = GetCurrentFallSpeed();
	for (int i = 0; i < MAX_FALLING_ENTITIES; ++i)
	{
		GameEntity& entity = m_entities[i];
		if (!entity.active) continue;
		if (entity.vy < fallSpeed) entity.vy = (entity.vy + 0.04f > fallSpeed + 1.5f) ? fallSpeed + 1.5f : entity.vy + 0.04f;
		entity.x += entity.vx;
		entity.y += entity.vy;

		float boundary = entity.usesRectCollider ? entity.halfWidth : entity.radius;
		if (entity.x < ROOM_LEFT + boundary) { entity.x = (float)(ROOM_LEFT + boundary); entity.vx = std::fabs(entity.vx); }
		if (entity.x > ROOM_RIGHT - boundary) { entity.x = (float)(ROOM_RIGHT - boundary); entity.vx = -std::fabs(entity.vx); }

		if (IsEntityOffScreen(entity)) { OnEntityFallOffScreen(entity); entity.active = false; }
	}
}

void MainScene::SpawnPopup(float x, float y, CrimeType crime)
{
	for (int i = 0; i < MAX_POPUP_TEXTS; ++i)
	{
		if (m_popups[i].active) continue;
		m_popups[i] = { true, x, y, -2.2f, 50, static_cast<int>(GetColor(255, 240, 120)), crime };
		return;
	}
}

void MainScene::UpdatePopups()
{
	for (int i = 0; i < MAX_POPUP_TEXTS; ++i)
	{
		if (!m_popups[i].active) continue;
		m_popups[i].y += m_popups[i].vy;
		m_popups[i].vy -= 0.04f;
		if (--m_popups[i].life <= 0) m_popups[i].active = false;
	}
}

void MainScene::UpdateIdleTimer()
{
	if (++m_idleTimerFrames >= IDLE_GAMEOVER_SEC * TARGET_FPS) EndGame(true);
}

void MainScene::UpdateFever()
{
	if (m_combo >= FEVER_COMBO_THRESHOLD && m_feverTimer <= 0) m_feverTimer = FEVER_DURATION_SEC * TARGET_FPS;
}

void MainScene::UpdateInput()
{
	const unsigned int mouseInput = GetMouseInput();
	// キャスト警告が出ないよう、比較対象も unsigned int として扱います
	const bool clicked = (mouseInput & MOUSE_INPUT_LEFT) && !((unsigned int)m_prevMouseInput & MOUSE_INPUT_LEFT);
	m_prevMouseInput = (int)mouseInput;
	if (!clicked) return;

	m_idleTimerFrames = 0;
	int mx, my; GetMousePoint(&mx, &my);

	// タップの基本エフェクト（青みがかったさわやかな光粒子）
	SpawnClickEffect(static_cast<float>(mx), static_cast<float>(my), ::GetColor(160, 210, 255));

	const float difficultyBonus = 1.0f + (GetDifficultyProgress() * 1.5f);

	for (int i = 0; i < MAX_FALLING_ENTITIES; ++i)
	{
		GameEntity& entity = m_entities[i];
		if (!entity.active || !IsClickOnEntity(entity, static_cast<float>(mx), static_cast<float>(my))) continue;

		entity.active = false;
		m_hitFlashTimer = 8;

		if (entity.kind == EntityKind::Obstacle)
		{
			m_score = (m_score < OBSTACLE_PENALTY_SCORE) ? 0 : (m_score - OBSTACLE_PENALTY_SCORE);
			m_timeLeftSec = (m_timeLeftSec < OBSTACLE_PENALTY_TIME_SEC) ? 0 : (m_timeLeftSec - OBSTACLE_PENALTY_TIME_SEC);
			m_combo = 0; m_comboTimer = 0;
			SpawnPopup(entity.x, entity.y, CrimeType::Trap);
			// 障害物クリックペナルティ時：赤色の飛び散る火花粒子
			SpawnClickEffect(entity.x, entity.y, ::GetColor(255, 60, 80));
		}
		else
		{
			int base = (entity.kind == EntityKind::Animal) ? 120 : 35;
			++m_combo;
			m_comboTimer = TARGET_FPS * 2;
			m_maxCombo = (m_combo > m_maxCombo) ? m_combo : m_maxCombo;
			base += (m_combo * (entity.kind == EntityKind::Animal ? 15 : 5));

			int clampedCombo = (m_combo > 50) ? 50 : m_combo;
			float comboMult = 1.0f + (static_cast<float>(clampedCombo) * 0.2f);
			int feverMult = (m_feverTimer > 0) ? 2 : 1;

			m_score += static_cast<int>(base * difficultyBonus * comboMult * feverMult);
			SpawnPopup(entity.x, entity.y, entity.crime);
			// 成功時：オブジェクトの色に合わせた華やかな飛び散りエフェクト
			SpawnClickEffect(entity.x, entity.y, entity.bodyColor);
		}
	}
}

bool MainScene::IsClickOnEntity(const GameEntity& entity, float mx, float my) const
{
	const float cursorR = GameSession::GetCursorRadius();

	if (entity.usesRectCollider)
		return CheckHitCircleRect({ mx, my, cursorR }, MakeRectColliderFromCenter(entity.x, entity.y, entity.halfWidth, entity.halfHeight));

	float dx = mx - entity.x, dy = my - entity.y;
	const float combinedR = entity.radius + cursorR;
	return (dx * dx + dy * dy) <= combinedR * combinedR;
}

void MainScene::Draw()
{
	DrawBackground();
	DrawPlayArea();
	DrawEntities();
	DrawPopups();
	DrawParticles();
	DrawUI();
	// カスタムカーソルを最前面に描画
	UiMouse::DrawCursor(GameSession::GetCursorRadius(), true);
}

void MainScene::DrawBackground()
{
	for (int y = 0; y < SCREEN_HEIGHT; ++y) DrawLine(0, y, SCREEN_WIDTH, y, ::GetColor(10, 15, 20 + (y * 60 / SCREEN_HEIGHT)));
	for (int i = 0; i < MAX_STARS; ++i) DrawCircle(m_stars[i].x, m_stars[i].y, 1 + (i % 2), ::GetColor(m_stars[i].brightness, m_stars[i].brightness, m_stars[i].brightness), TRUE);
}

void MainScene::DrawPlayArea()
{
	DrawBox(ROOM_LEFT - 12, ROOM_TOP - 12, ROOM_RIGHT + 12, ROOM_BOTTOM + 12, ::GetColor(70, 45, 30), TRUE);
	DrawBox(ROOM_LEFT, ROOM_TOP, ROOM_RIGHT, ROOM_BOTTOM, ::GetColor(55, 38, 28), TRUE);
	DrawBox(ROOM_LEFT + 8, ROOM_TOP + 8, ROOM_RIGHT - 8, ROOM_BOTTOM - 8, ::GetColor(95, 70, 55), TRUE);
}

void MainScene::DrawEntities()
{
	for (int i = 0; i < MAX_FALLING_ENTITIES; ++i)
	{
		const GameEntity& e = m_entities[i];
		if (!e.active) continue;
		if (e.usesRectCollider) {
			DrawBox((int)(e.x - e.halfWidth), (int)(e.y - e.halfHeight), (int)(e.x + e.halfWidth), (int)(e.y + e.halfHeight), e.bodyColor, TRUE);
		}
		else {
			DrawCircle((int)e.x, (int)e.y, (int)e.radius, e.bodyColor, TRUE);
		}
	}
}

void MainScene::DrawPopups()
{
	SetFontSize(22);
	for (int i = 0; i < MAX_POPUP_TEXTS; ++i) if (m_popups[i].active) DrawFormatString((int)m_popups[i].x - 80, (int)m_popups[i].y, m_popups[i].color, GetCrimeString(m_popups[i].crime));
}

void MainScene::DrawUI()
{
	SetFontSize(24);
	DrawFormatString(20, 20, ::GetColor(220, 230, 255), STR_MAIN_TIMER, m_timeLeftSec);
	DrawFormatString(220, 20, ::GetColor(255, 230, 180), STR_MAIN_SCORE, m_score);

	GameDifficulty diff = GameSession::GetDifficulty();
	const wchar_t* diffStr = (diff == GameDifficulty::Easy) ? STR_DIFF_EASY : (diff == GameDifficulty::Hell) ? STR_DIFF_HELL : STR_DIFF_NORMAL;
	DrawFormatString(1000, 20, ::GetColor(200, 255, 200), L"%s", diffStr);

	if (m_combo >= 2) DrawFormatString(24, 86, ::GetColor(255, 180, 120), STR_MAIN_COMBO, m_combo);
	if (m_feverTimer > 0) DrawFormatString(SCREEN_WIDTH / 2 - 140, 24, ::GetColor(255, 120, 200), STR_MAIN_FEVER);
}

void MainScene::SpawnClickEffect(float x, float y, int color)
{
	const int count = 12;
	for (int k = 0; k < count; ++k)
	{
		int slot = -1;
		for (int i = 0; i < MAX_CLICK_PARTICLES; ++i)
		{
			if (!m_particles[i].active)
			{
				slot = i;
				break;
			}
		}
		if (slot < 0) break;

		float angle = RandomRange(0.0f, 360.0f) * 3.14159265f / 180.0f;
		float speed = RandomRange(2.0f, 6.0f);
		int maxLife = 20 + GetRand(15);

		m_particles[slot].active = true;
		m_particles[slot].x = x;
		m_particles[slot].y = y;
		m_particles[slot].vx = std::cos(angle) * speed;
		m_particles[slot].vy = std::sin(angle) * speed;
		m_particles[slot].color = color;
		m_particles[slot].life = maxLife;
		m_particles[slot].maxLife = maxLife;
		m_particles[slot].size = RandomRange(4.0f, 9.0f);
	}
}

void MainScene::UpdateParticles()
{
	for (int i = 0; i < MAX_CLICK_PARTICLES; ++i)
	{
		ClickParticle& p = m_particles[i];
		if (!p.active) continue;

		p.x += p.vx;
		p.y += p.vy;
		p.vx *= 0.92f;
		p.vy *= 0.92f;

		if (--p.life <= 0)
		{
			p.active = false;
		}
	}
}

void MainScene::DrawParticles()
{
	for (int i = 0; i < MAX_CLICK_PARTICLES; ++i)
	{
		const ClickParticle& p = m_particles[i];
		if (!p.active) continue;

		float alphaPercent = (float)p.life / p.maxLife;
		int alpha = (int)(alphaPercent * 255);
		float currentSize = p.size * alphaPercent;

		SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);
		DrawCircle((int)p.x, (int)p.y, (int)(currentSize + 0.5f), p.color, TRUE);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

SceneID MainScene::GetNextSceneID() const { return m_requestGoResult ? SceneID::Result : SceneID::None; }