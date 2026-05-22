#include "MainScene.h"

#include "Collision.h"
#include "GameConfig.h"
#include "GameStrings.h"
#include "GameSession.h"
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

	for (int i = 0; i < MAX_FALLING_ENTITIES; ++i)
	{
		m_entities[i].active = false;
	}
	for (int i = 0; i < MAX_POPUP_TEXTS; ++i)
	{
		m_popups[i].active = false;
	}

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
	if (GAME_DURATION_SEC <= 0)
	{
		return 1.0f;
	}
	const float elapsed = static_cast<float>(GAME_DURATION_SEC - m_timeLeftSec);
	return elapsed / static_cast<float>(GAME_DURATION_SEC);
}

float MainScene::GetCurrentFallSpeed() const
{
	const float progress = GetDifficultyProgress();
	float speed = FALL_SPEED_BASE + (FALL_SPEED_MAX - FALL_SPEED_BASE) * progress;
	if (GameSession::GetDifficulty() == GameDifficulty::Easy)
	{
		speed *= 0.6f;
	}
	return speed;
}

int MainScene::GetSpawnIntervalFrames() const
{
	const float progress = GetDifficultyProgress();
	int interval = static_cast<int>(
		SPAWN_INTERVAL_MAX_FRAMES - (SPAWN_INTERVAL_MAX_FRAMES - SPAWN_INTERVAL_MIN_FRAMES) * progress);
	
	if (GameSession::GetDifficulty() == GameDifficulty::Easy)
	{
		interval = static_cast<int>(interval * 1.5f);
	}
	return interval;
}

int MainScene::GetMaxConcurrentObjects() const
{
	const float progress = GetDifficultyProgress();
	const float count = static_cast<float>(MAX_ON_SCREEN_MIN)
		+ (static_cast<float>(MAX_ON_SCREEN_MAX) - static_cast<float>(MAX_ON_SCREEN_MIN)) * progress;
	return static_cast<int>(count + 0.5f);
}

int MainScene::CountActiveEntities() const
{
	int count = 0;
	for (int i = 0; i < MAX_FALLING_ENTITIES; ++i)
	{
		if (m_entities[i].active)
		{
			++count;
		}
	}
	return count;
}

bool MainScene::TrySpawnEntity()
{
	if (CountActiveEntities() >= GetMaxConcurrentObjects())
	{
		return false;
	}

	int slot = -1;
	for (int i = 0; i < MAX_FALLING_ENTITIES; ++i)
	{
		if (!m_entities[i].active)
		{
			slot = i;
			break;
		}
	}
	if (slot < 0)
	{
		return false;
	}

	GameEntity& entity = m_entities[slot];
	int spawnPercent = OBSTACLE_SPAWN_PERCENT;
	if (GameSession::GetDifficulty() == GameDifficulty::Easy)
	{
		spawnPercent /= 2;
	}
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
		entity.radius = 0.0f;
		entity.x = RandomRange(
			static_cast<float>(ROOM_LEFT) + entity.halfWidth,
			static_cast<float>(ROOM_RIGHT) - entity.halfWidth);
		entity.y = static_cast<float>(ROOM_TOP) - entity.halfHeight - 8.0f;
		entity.bodyColor = GetColor(200, 55, 75);
		entity.accentColor = GetColor(130, 25, 40);
	}
	else if (GetRand(100) < 35)
	{
		entity.kind = EntityKind::Item;
		entity.crime = CrimeType::Snack;
		entity.usesRectCollider = false;
		entity.radius = 18.0f + static_cast<float>(GetRand(10));
		entity.halfWidth = 0.0f;
		entity.halfHeight = 0.0f;
		entity.x = RandomRange(
			static_cast<float>(ROOM_LEFT) + entity.radius,
			static_cast<float>(ROOM_RIGHT) - entity.radius);
		entity.y = static_cast<float>(ROOM_TOP) - entity.radius - 8.0f;
		entity.bodyColor = GetColor(255, 220, 120);
		entity.accentColor = GetColor(200, 150, 60);
	}
	else
	{
		entity.kind = EntityKind::Animal;
		entity.crime = static_cast<CrimeType>(GetRand(4));
		entity.usesRectCollider = false;
		entity.radius = 30.0f + static_cast<float>(GetRand(12));
		entity.halfWidth = 0.0f;
		entity.halfHeight = 0.0f;
		entity.x = RandomRange(
			static_cast<float>(ROOM_LEFT) + entity.radius,
			static_cast<float>(ROOM_RIGHT) - entity.radius);
		entity.y = static_cast<float>(ROOM_TOP) - entity.radius - 8.0f;
		entity.bodyColor = GetColor(240, 210 + GetRand(40), 180 + GetRand(50));
		entity.accentColor = GetColor(80, 60, 50);
	}

	return true;
}

void MainScene::UpdateSpawning()
{
	if (m_spawnTimer > 0)
	{
		--m_spawnTimer;
		return;
	}

	if (CountActiveEntities() >= GetMaxConcurrentObjects())
	{
		m_spawnTimer = 4;
		return;
	}

	if (TrySpawnEntity())
	{
		m_spawnTimer = GetSpawnIntervalFrames();
	}
	else
	{
		m_spawnTimer = 8;
	}
}

void MainScene::Update()
{
	if (m_timeLeftSec <= 0)
	{
		EndGame(false);
		return;
	}

	UpdateTimer();
	UpdateSpawning();
	UpdateEntities();
	UpdatePopups();
	UpdateInput();
	UpdateIdleTimer();
	UpdateFever();

	if (m_comboTimer > 0)
	{
		--m_comboTimer;
		if (m_comboTimer <= 0)
		{
			m_combo = 0;
		}
	}

	if (m_hitFlashTimer > 0) --m_hitFlashTimer;
	if (m_feverTimer > 0)      --m_feverTimer;
}

void MainScene::EndGame(bool isGameOver)
{
	GameSession::SetLastResult(m_score, m_maxCombo, isGameOver);
	m_requestGoResult = true;
}

void MainScene::UpdateTimer()
{
	++m_secondCounter;
	if (m_secondCounter >= TARGET_FPS)
	{
		m_secondCounter = 0;
		--m_timeLeftSec;
	}
}

bool MainScene::IsEntityOffScreen(const GameEntity& entity) const
{
	if (entity.usesRectCollider)
	{
		return entity.y - entity.halfHeight > static_cast<float>(ROOM_BOTTOM) + 40.0f;
	}
	return entity.y - entity.radius > static_cast<float>(ROOM_BOTTOM) + 40.0f;
}

void MainScene::OnEntityFallOffScreen(GameEntity& entity)
{
	// 妨害は落ちて消えても何もしない（クリックしなかった正解扱い）
	if (entity.kind == EntityKind::Obstacle)
	{
		return;
	}

	const int penalty = (entity.kind == EntityKind::Animal)
		? MISS_ANIMAL_PENALTY_SCORE
		: MISS_ITEM_PENALTY_SCORE;

	m_score -= penalty;
	if (m_score < 0)
	{
		m_score = 0;
	}
	m_combo = 0;
	m_comboTimer = 0;
}

void MainScene::UpdateEntities()
{
	const float fallSpeed = GetCurrentFallSpeed();

	for (int i = 0; i < MAX_FALLING_ENTITIES; ++i)
	{
		GameEntity& entity = m_entities[i];
		if (!entity.active)
		{
			continue;
		}

		// 残り時間が減るほど既存オブジェクトも少しずつ加速
		if (entity.vy < fallSpeed)
		{
			entity.vy += 0.04f;
			if (entity.vy > fallSpeed + 1.5f)
			{
				entity.vy = fallSpeed + 1.5f;
			}
		}

		entity.x += entity.vx;
		entity.y += entity.vy;

		if (entity.usesRectCollider)
		{
			const float left = static_cast<float>(ROOM_LEFT) + entity.halfWidth;
			const float right = static_cast<float>(ROOM_RIGHT) - entity.halfWidth;
			if (entity.x < left)  { entity.x = left;  entity.vx = std::fabs(entity.vx); }
			if (entity.x > right) { entity.x = right; entity.vx = -std::fabs(entity.vx); }
		}
		else
		{
			const float left = static_cast<float>(ROOM_LEFT) + entity.radius;
			const float right = static_cast<float>(ROOM_RIGHT) - entity.radius;
			if (entity.x < left)  { entity.x = left;  entity.vx = std::fabs(entity.vx); }
			if (entity.x > right) { entity.x = right; entity.vx = -std::fabs(entity.vx); }
		}

		if (IsEntityOffScreen(entity))
		{
			OnEntityFallOffScreen(entity);
			entity.active = false;
		}
	}
}

void MainScene::SpawnPopup(float x, float y, CrimeType crime)
{
	for (int i = 0; i < MAX_POPUP_TEXTS; ++i)
	{
		if (m_popups[i].active) continue;

		m_popups[i].active = true;
		m_popups[i].x = x;
		m_popups[i].y = y;
		m_popups[i].vy = -2.2f;
		m_popups[i].life = 50;
		m_popups[i].color = GetColor(255, 240, 120);
		m_popups[i].crime = crime;
		return;
	}
}

void MainScene::UpdatePopups()
{
	for (int i = 0; i < MAX_POPUP_TEXTS; ++i)
	{
		PopupText& popup = m_popups[i];
		if (!popup.active) continue;

		popup.y += popup.vy;
		popup.vy -= 0.04f;
		--popup.life;
		if (popup.life <= 0) popup.active = false;
	}
}

void MainScene::UpdateIdleTimer()
{
	++m_idleTimerFrames;
	if (m_idleTimerFrames >= IDLE_GAMEOVER_SEC * TARGET_FPS)
	{
		EndGame(true);
	}
}

void MainScene::UpdateFever()
{
	if (m_combo >= FEVER_COMBO_THRESHOLD && m_feverTimer <= 0)
	{
		m_feverTimer = FEVER_DURATION_SEC * TARGET_FPS;
	}
}

void MainScene::UpdateInput()
{
	const int mouseInput = GetMouseInput();
	const bool clicked =
		(mouseInput & MOUSE_INPUT_LEFT) != 0 &&
		(m_prevMouseInput & MOUSE_INPUT_LEFT) == 0;
	m_prevMouseInput = mouseInput;

	if (!clicked)
	{
		return;
	}

	m_idleTimerFrames = 0;

	int mouseX = 0;
	int mouseY = 0;
	GetMousePoint(&mouseX, &mouseY);
	const float roomMouseX = static_cast<float>(mouseX);
	const float roomMouseY = static_cast<float>(mouseY);

	for (int i = 0; i < MAX_FALLING_ENTITIES; ++i)
	{
		GameEntity& entity = m_entities[i];
		if (!entity.active || !IsClickOnEntity(entity, roomMouseX, roomMouseY))
		{
			continue;
		}

		entity.active = false;
		m_hitFlashTimer = 8;

		if (entity.kind == EntityKind::Obstacle)
		{
			m_score -= OBSTACLE_PENALTY_SCORE;
			if (m_score < 0) m_score = 0;
			m_timeLeftSec -= OBSTACLE_PENALTY_TIME_SEC;
			if (m_timeLeftSec < 0) m_timeLeftSec = 0;
			m_combo = 0;
			m_comboTimer = 0;
			SpawnPopup(entity.x, entity.y, CrimeType::Trap);
			continue;
		}

		int gainedScore = 0;
		const int feverBonus = (m_feverTimer > 0) ? 2 : 1;

		if (entity.kind == EntityKind::Animal)
		{
			gainedScore = 120 * feverBonus;
			++m_combo;
			m_comboTimer = TARGET_FPS * 2;
			if (m_combo > m_maxCombo) m_maxCombo = m_combo;
			gainedScore += m_combo * 15;
			SpawnPopup(entity.x, entity.y - entity.radius - 10.0f, entity.crime);
		}
		else
		{
			gainedScore = 35 * feverBonus;
			++m_combo;
			m_comboTimer = TARGET_FPS * 2;
			if (m_combo > m_maxCombo) m_maxCombo = m_combo;
			gainedScore += m_combo * 5;
			SpawnPopup(entity.x, entity.y, entity.crime);
		}

		m_score += gainedScore;
	}
}

bool MainScene::IsClickOnEntity(
	const GameEntity& entity,
	float mouseX,
	float mouseY) const
{
	if (entity.usesRectCollider)
	{
		RectCollider rect = MakeRectColliderFromCenter(
			entity.x, entity.y, entity.halfWidth, entity.halfHeight);
		CircleCollider clickPoint = MakeCircleCollider(mouseX, mouseY, 6.0f);
		return CheckHitCircleRect(clickPoint, rect);
	}

	const float dx = mouseX - entity.x;
	const float dy = mouseY - entity.y;
	return (dx * dx + dy * dy) <= (entity.radius + 6.0f) * (entity.radius + 6.0f);
}

void MainScene::Draw()
{
	DrawBackground();
	DrawPlayArea();
	DrawEntities();
	DrawPopups();
	DrawUI();
}

void MainScene::DrawBackground()
{
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		const int blue = 20 + (y * 60 / SCREEN_HEIGHT);
		DrawLine(0, y, SCREEN_WIDTH, y, GetColor(10, 15, blue));
	}
	for (int i = 0; i < MAX_STARS; ++i)
	{
		const int c = m_stars[i].brightness;
		DrawCircle(m_stars[i].x, m_stars[i].y, 1 + (i % 2), GetColor(c, c, c), TRUE);
	}
}

void MainScene::DrawPlayArea()
{
	DrawBox(ROOM_LEFT - 12, ROOM_TOP - 12, ROOM_RIGHT + 12, ROOM_BOTTOM + 12, GetColor(70, 45, 30), TRUE);
	DrawBox(ROOM_LEFT, ROOM_TOP, ROOM_RIGHT, ROOM_BOTTOM, GetColor(55, 38, 28), TRUE);
	DrawBox(ROOM_LEFT + 8, ROOM_TOP + 8, ROOM_RIGHT - 8, ROOM_BOTTOM - 8, GetColor(95, 70, 55), TRUE);

	const int frame = GetNowCount();
	for (int i = 0; i < 3; ++i)
	{
		const int lx = ROOM_LEFT + 180 + i * 260;
		const int ly = ROOM_TOP + 80;
		const int glow = 40 + static_cast<int>(10.0 * std::sin(frame * 0.05 + i));
		DrawCircle(lx, ly, 28, GetColor(255, 220, 120), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 50);
		DrawCircle(lx, ly, 55 + glow / 2, GetColor(255, 200, 80), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// 落下開始ライン
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 60);
	DrawBox(ROOM_LEFT, ROOM_TOP - 4, ROOM_RIGHT, ROOM_TOP + 2, GetColor(180, 220, 255), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void MainScene::DrawEntities()
{
	for (int i = 0; i < MAX_FALLING_ENTITIES; ++i)
	{
		const GameEntity& entity = m_entities[i];
		if (!entity.active) continue;

		const int sx = static_cast<int>(entity.x);
		const int sy = static_cast<int>(entity.y);

		if (entity.usesRectCollider)
		{
			DrawBox(
				sx - static_cast<int>(entity.halfWidth),
				sy - static_cast<int>(entity.halfHeight),
				sx + static_cast<int>(entity.halfWidth),
				sy + static_cast<int>(entity.halfHeight),
				entity.bodyColor,
				TRUE);
			DrawBox(
				sx - static_cast<int>(entity.halfWidth) + 4,
				sy - static_cast<int>(entity.halfHeight) + 4,
				sx + static_cast<int>(entity.halfWidth) - 4,
				sy + static_cast<int>(entity.halfHeight) - 4,
				entity.accentColor,
				FALSE);
		}
		else if (entity.kind == EntityKind::Item)
		{
			const int radius = static_cast<int>(entity.radius);
			DrawCircle(sx, sy, radius, entity.bodyColor, TRUE);
			DrawCircle(sx, sy - radius / 2, radius / 2, entity.accentColor, TRUE);
		}
		else
		{
			const int radius = static_cast<int>(entity.radius);
			DrawCircle(sx, sy, radius, entity.bodyColor, TRUE);
			DrawCircle(sx - radius / 2, sy - radius, radius / 3, entity.bodyColor, TRUE);
			DrawCircle(sx + radius / 2, sy - radius, radius / 3, entity.bodyColor, TRUE);
			DrawCircle(sx - 8, sy - 4, 4, entity.accentColor, TRUE);
			DrawCircle(sx + 8, sy - 4, 4, entity.accentColor, TRUE);
		}
	}
}

void MainScene::DrawPopups()
{
	SetFontSize(22);
	for (int i = 0; i < MAX_POPUP_TEXTS; ++i)
	{
		const PopupText& popup = m_popups[i];
		if (!popup.active) continue;

		DrawFormatString(
			static_cast<int>(popup.x) - 80,
			static_cast<int>(popup.y),
			popup.color,
			GetCrimeString(popup.crime));
	}
}

void MainScene::DrawUI()
{
	SetFontSize(24);
	DrawFormatString(20, 20, GetColor(220, 230, 255), STR_MAIN_TIMER, m_timeLeftSec);
	DrawFormatString(220, 20, GetColor(255, 230, 180), STR_MAIN_SCORE, m_score);
	
	const wchar_t* diffStr = GameSession::GetDifficulty() == GameDifficulty::Easy ? STR_DIFF_EASY : STR_DIFF_NORMAL;
	DrawFormatString(1000, 20, GetColor(200, 255, 200), L"%s", diffStr);

	if (m_combo >= 2)
	{
		SetFontSize(22);
		DrawFormatString(24, 86, GetColor(255, 180, 120), STR_MAIN_COMBO, m_combo);
	}

	if (m_feverTimer > 0)
	{
		SetFontSize(30);
		DrawFormatString(SCREEN_WIDTH / 2 - 140, 24, GetColor(255, 120, 200), STR_MAIN_FEVER);
	}

	const int idleSecLeft = IDLE_GAMEOVER_SEC - (m_idleTimerFrames / TARGET_FPS);
	if (idleSecLeft <= 3 && idleSecLeft > 0)
	{
		SetFontSize(24);
		DrawFormatString(SCREEN_WIDTH / 2 - 260, SCREEN_HEIGHT - 100, GetColor(255, 100, 100), STR_MAIN_IDLE_WARN, idleSecLeft);
	}

	SetFontSize(18);
	DrawFormatString(24, SCREEN_HEIGHT - 36, GetColor(180, 180, 200), STR_MAIN_HINT);

	// 残り時間が少ないほど難易度上昇の目安表示
	if (m_timeLeftSec <= RUSH_WARNING_SEC)
	{
		SetFontSize(22);
		const int rushColor = (GetNowCount() / 10) % 2 == 0
			? GetColor(255, 180, 100) : GetColor(255, 120, 120);
		DrawFormatString(SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT - 70, rushColor, STR_MAIN_RUSH);
	}

	if (m_hitFlashTimer > 0)
	{
		SetFontSize(22);
		DrawFormatString(SCREEN_WIDTH / 2 - 40, 120, GetColor(255, 255, 180), STR_MAIN_HIT_FLASH);
	}
}

SceneID MainScene::GetNextSceneID() const
{
	if (m_requestGoResult) return SceneID::Result;
	return SceneID::None;
}
