#include "MainScene.h"

#include "Collision.h"
#include "GameConfig.h"
#include "GameStrings.h"
#include "GameSession.h"
#include "UiMouse.h"
#include "DxLib.h"
#include "LoadingScene.h"

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

	// ポーズ関連
	m_pauseState     = PauseState::None;
	m_requestGoTitle = false;
	m_prevEscKey     = false;
	m_igSensDragging = false;
	m_igCurDragging  = false;
	m_igSensValue    = GameSession::GetMouseSensitivity();
	m_igCurValue     = GameSession::GetCursorRadius();

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
	}
	else if (GetRand(100) < 35)
	{
		entity.kind = EntityKind::Item;
		entity.crime = CrimeType::Snack;
		entity.usesRectCollider = false;
		entity.radius = 18.0f + static_cast<float>(GetRand(10));
		entity.x = RandomRange(static_cast<float>(ROOM_LEFT) + entity.radius, static_cast<float>(ROOM_RIGHT) - entity.radius);
		entity.y = static_cast<float>(ROOM_TOP) - entity.radius - 8.0f;
	}
	else
	{
		entity.kind = EntityKind::Animal;
		entity.crime = static_cast<CrimeType>(GetRand(4));
		entity.usesRectCollider = false;
		entity.radius = 30.0f + static_cast<float>(GetRand(12));
		entity.x = RandomRange(static_cast<float>(ROOM_LEFT) + entity.radius, static_cast<float>(ROOM_RIGHT) - entity.radius);
		entity.y = static_cast<float>(ROOM_TOP) - entity.radius - 8.0f;
	}

	// 犯罪の種類に応じた固有の配色を設定（エフェクトや描画で使用）
	switch (entity.crime)
	{
	case CrimeType::Hack:
		entity.bodyColor = ::GetColor(0, 245, 255);
		entity.accentColor = ::GetColor(0, 120, 200);
		break;
	case CrimeType::Bomb:
		entity.bodyColor = ::GetColor(255, 190, 0);
		entity.accentColor = ::GetColor(255, 90, 0);
		break;
	case CrimeType::Ritual:
		entity.bodyColor = ::GetColor(255, 0, 150);
		entity.accentColor = ::GetColor(170, 50, 255);
		break;
	case CrimeType::Snack:
		entity.bodyColor = ::GetColor(255, 130, 170);
		entity.accentColor = ::GetColor(255, 215, 80);
		break;
	case CrimeType::Trap:
		entity.bodyColor = ::GetColor(220, 30, 30);
		entity.accentColor = ::GetColor(255, 215, 0);
		break;
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
	// ポーズ処理（最優先）
	UpdatePauseMenu();
	if (m_pauseState != PauseState::None) return;

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
	// デバッグ: 時間停止
	if (GameSession::GetDebugModeEnabled() && GameSession::GetDebugInfiniteTime()) return;
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
	// デバッグ: コンボ維持（画面外に出てもコンボが切れない）
	if (GameSession::GetDebugModeEnabled() && GameSession::GetDebugNoComboBreak()) return;
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

	// ==============================
	// デバッグ: 中央付近オートクリック
	// ==============================
	if (GameSession::GetDebugModeEnabled() && GameSession::GetDebugAutoClick())
	{
		const float difficultyBonus = 1.0f + (GetDifficultyProgress() * 1.5f);
		// Auto-click debug: simulate a click on all entities (obstacles excluded) with full scoring logic
		for (int i = 0; i < MAX_FALLING_ENTITIES; ++i)
		{
			GameEntity& entity = m_entities[i];
			if (!entity.active) continue;

			// 妨害オブジェクトはオートクリックの対象外
			if (entity.kind == EntityKind::Obstacle) continue;

			// Apply same handling as manual click
			entity.active = false;
			m_idleTimerFrames = 0;
			m_hitFlashTimer = 4;

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
				SpawnClickEffect(entity.x, entity.y, entity.bodyColor);
			}
		}
	}

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
			// デバッグ: 罠無効
			if (GameSession::GetDebugModeEnabled() && GameSession::GetDebugNoTrapPenalty())
			{
				SpawnClickEffect(entity.x, entity.y, ::GetColor(100, 255, 100));
				continue;
			}
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

	// ポーズオーバーレイ（最前面）
	if (m_pauseState == PauseState::Menu)     DrawPauseOverlay();
	else if (m_pauseState == PauseState::Settings) DrawInGameSettings();

	// カスタムカーソルを最前面に描画
	UiMouse::DrawCursor(GameSession::GetCursorRadius(), true);
}

void MainScene::DrawBackground()
{
	// 1. ベースの宇宙グラデーション背景
	for (int y = 0; y < SCREEN_HEIGHT; ++y) {
		DrawLine(0, y, SCREEN_WIDTH, y, ::GetColor(8, 12, 18 + (y * 50 / SCREEN_HEIGHT)));
	}
	// 2. 星々
	for (int i = 0; i < MAX_STARS; ++i) {
		DrawCircle(m_stars[i].x, m_stars[i].y, 1 + (i % 2), ::GetColor(m_stars[i].brightness, m_stars[i].brightness, m_stars[i].brightness), TRUE);
	}

	// 3. 画面の四隅のSFチックなHUDブラケット
	unsigned int hudColor = ::GetColor(0, 160, 255);
	int bracketLen = 20;
	// 左上
	DrawLine(5, 5, 5 + bracketLen, 5, hudColor);
	DrawLine(5, 5, 5, 5 + bracketLen, hudColor);
	// 右上
	DrawLine(SCREEN_WIDTH - 5, 5, SCREEN_WIDTH - 5 - bracketLen, 5, hudColor);
	DrawLine(SCREEN_WIDTH - 5, 5, SCREEN_WIDTH - 5, 5 + bracketLen, hudColor);
	// 左下
	DrawLine(5, SCREEN_HEIGHT - 5, 5 + bracketLen, SCREEN_HEIGHT - 5, hudColor);
	DrawLine(5, SCREEN_HEIGHT - 5, 5, SCREEN_HEIGHT - 5 - bracketLen, hudColor);
	// 右下
	DrawLine(SCREEN_WIDTH - 5, SCREEN_HEIGHT - 5, SCREEN_WIDTH - 5 - bracketLen, SCREEN_HEIGHT - 5, hudColor);
	DrawLine(SCREEN_WIDTH - 5, SCREEN_HEIGHT - 5, SCREEN_WIDTH - 5, SCREEN_HEIGHT - 5 - bracketLen, hudColor);

	// 4. 左側パネル（サイバーセキュリティ・モニター）
	{
		// パネル半透明背景
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 50);
		DrawBox(10, ROOM_TOP, ROOM_LEFT - 20, ROOM_BOTTOM, ::GetColor(15, 25, 40), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		
		// パネル外枠
		DrawBox(10, ROOM_TOP, ROOM_LEFT - 20, ROOM_BOTTOM, ::GetColor(0, 100, 180), FALSE);
		DrawBox(12, ROOM_TOP + 2, ROOM_LEFT - 22, ROOM_BOTTOM - 2, ::GetColor(0, 60, 120), FALSE);

		// アナライザー表示
		int startY = ROOM_TOP + 30;
		SetFontSize(14);
		DrawString(20, startY - 20, L"SYSTEM ANALYZER", ::GetColor(0, 220, 255));
		
		// 5本のアニメーションウェーブバー
		for (int j = 0; j < 5; ++j)
		{
			float speed = 0.002f + j * 0.0008f;
			float val = std::sin((float)GetNowCount() * speed) * 0.5f + 0.5f;
			int barW = (int)(val * 115.0f);
			unsigned int barCol = ::GetColor(0, 160 + (j * 18), 255);
			// バックバー
			DrawBox(20, startY + (j * 16), 170, startY + (j * 16) + 6, ::GetColor(20, 35, 50), TRUE);
			// アクティブバー
			DrawBox(20, startY + (j * 16), 20 + barW, startY + (j * 16) + 6, barCol, TRUE);
		}

		// ステータス情報テキスト
		int textY = startY + 105;
		DrawString(20, textY, L"SECURE SHIELD", ::GetColor(140, 170, 210));
		DrawString(20, textY + 16, L"STATUS: ACTIVE", ::GetColor(0, 255, 130));
		DrawFormatString(20, textY + 32, ::GetColor(140, 170, 210), L"THREATS: %2d", CountActiveEntities());

		// 脅威レベルの動的変化
		unsigned int threatCol = ::GetColor(0, 255, 100);
		const wchar_t* threatText = L"LEVEL: SAFE";
		if (m_feverTimer > 0)
		{
			threatCol = ::GetColor(255, 0, 130);
			threatText = L"LEVEL: OVERLOAD";
		}
		else if (CountActiveEntities() > 10)
		{
			threatCol = ::GetColor(255, 120, 0);
			threatText = L"LEVEL: WARNING";
		}
		DrawString(20, textY + 48, threatText, threatCol);

		// 回転するサイバーレーダー
		int radarX = 95;
		int radarY = ROOM_BOTTOM - 75;
		int radarR = 48;
		float rAngle = (float)GetNowCount() * 0.0012f;
		
		DrawCircle(radarX, radarY, radarR, ::GetColor(0, 90, 140), FALSE);
		DrawCircle(radarX, radarY, radarR - 15, ::GetColor(0, 70, 110), FALSE);
		DrawCircle(radarX, radarY, radarR - 30, ::GetColor(0, 50, 80), FALSE);
		
		int sweepX = radarX + (int)(std::cos(rAngle) * radarR);
		int sweepY = radarY + (int)(std::sin(rAngle) * radarR);
		DrawLine(radarX, radarY, sweepX, sweepY, ::GetColor(0, 255, 255));
		
		// ターゲット検知点ハイライト（点滅）
		if (CountActiveEntities() > 0) {
			int targetAngle = (GetNowCount() / 300) % 4;
			int targetX = radarX + (int)(std::cos(targetAngle * 1.57f) * (radarR - 10));
			int targetY = radarY + (int)(std::sin(targetAngle * 1.57f) * (radarR - 10));
			DrawCircle(targetX, targetY, 3, ::GetColor(255, 80, 80), TRUE);
		}
	}

	// 5. 右側パネル（神秘のオカルトエンジン）
	{
		// パネル半透明背景
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 50);
		DrawBox(ROOM_RIGHT + 20, ROOM_TOP, SCREEN_WIDTH - 10, ROOM_BOTTOM, ::GetColor(25, 15, 40), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		
		// パネル外枠
		DrawBox(ROOM_RIGHT + 20, ROOM_TOP, SCREEN_WIDTH - 10, ROOM_BOTTOM, ::GetColor(150, 40, 230), FALSE);
		DrawBox(ROOM_RIGHT + 22, ROOM_TOP + 2, SCREEN_WIDTH - 12, ROOM_BOTTOM - 2, ::GetColor(100, 20, 170), FALSE);

		// 回転魔法陣ディスプレイ
		int magicX = ROOM_RIGHT + 95;
		int magicY = ROOM_TOP + 80;
		int magicR = 52;
		float mAngle = (float)GetNowCount() * -0.0008f;
		
		DrawCircle(magicX, magicY, magicR, ::GetColor(120, 0, 180), FALSE);
		DrawCircle(magicX, magicY, magicR - 8, ::GetColor(90, 0, 140), FALSE);
		DrawCircle(magicX, magicY, magicR - 30, ::GetColor(240, 0, 150), FALSE);
		
		// 内側の回転する三角形
		int mxP[3], myP[3];
		for (int k = 0; k < 3; ++k)
		{
			float a = mAngle + k * (2.0f * 3.14159265f / 3.0f);
			mxP[k] = magicX + (int)(std::cos(a) * (magicR - 4));
			myP[k] = magicY + (int)(std::sin(a) * (magicR - 4));
		}
		DrawLine(mxP[0], myP[0], mxP[1], myP[1], ::GetColor(180, 40, 255));
		DrawLine(mxP[1], myP[1], mxP[2], myP[2], ::GetColor(180, 40, 255));
		DrawLine(mxP[2], myP[2], mxP[0], myP[0], ::GetColor(180, 40, 255));

		// 属性エネルギーゲージ
		int gaugeY = magicY + 95;
		SetFontSize(14);
		DrawString(ROOM_RIGHT + 30, gaugeY - 20, L"AURA ENERGY", ::GetColor(230, 190, 255));
		
		const wchar_t* elemNames[] = { L"FIR", L"ICE", L"LTG" };
		unsigned int elemCols[] = { ::GetColor(255, 80, 40), ::GetColor(40, 190, 255), ::GetColor(255, 220, 40) };
		for (int j = 0; j < 3; ++j)
		{
			float phase = j * 1.5f;
			float level = std::sin((float)GetNowCount() * 0.0015f + phase) * 0.35f + 0.65f;
			int barH = (int)(level * 90.0f);
			
			// ラベル
			DrawString(ROOM_RIGHT + 32 + (j * 44), gaugeY, elemNames[j], elemCols[j]);
			// 背景スロット
			DrawBox(ROOM_RIGHT + 36 + (j * 44), gaugeY + 16, ROOM_RIGHT + 36 + (j * 44) + 10, gaugeY + 110, ::GetColor(35, 25, 45), TRUE);
			// 動的エネルギーゲージ
			DrawBox(ROOM_RIGHT + 36 + (j * 44), gaugeY + 110 - barH, ROOM_RIGHT + 36 + (j * 44) + 10, gaugeY + 110, elemCols[j], TRUE);
		}

		// 魔術ステータス
		int rightTextY = gaugeY + 125;
		DrawString(ROOM_RIGHT + 30, rightTextY, L"MAGIC SYNERGY", ::GetColor(160, 140, 180));
		DrawString(ROOM_RIGHT + 30, rightTextY + 16, L"INTEGRITY: 98%", ::GetColor(180, 40, 255));
		DrawString(ROOM_RIGHT + 30, rightTextY + 32, L"STATUS: STABLE", ::GetColor(0, 255, 130));
	}
}

void MainScene::DrawPlayArea()
{
	// 金属風の重厚なベース外枠
	DrawBox(ROOM_LEFT - 14, ROOM_TOP - 14, ROOM_RIGHT + 14, ROOM_BOTTOM + 14, ::GetColor(30, 35, 45), TRUE);
	
	// ネオンで輝くバリア境界線（フィーバー中や警告中にダイナミックに変化）
	unsigned int borderColor = ::GetColor(0, 160, 255); // デフォルト：水色
	if (m_feverTimer > 0)
	{
		borderColor = ::GetColor(255, 50, 180); // フィーバー中：ピンク
	}
	else if (m_timeLeftSec <= 10)
	{
		borderColor = (GetNowCount() % 500 < 250) ? ::GetColor(255, 30, 30) : ::GetColor(100, 10, 10); // タイムアップ警告：赤点滅
	}
	
	// ネオンの多重グロー表現
	for (int i = 0; i < 4; ++i)
	{
		int alpha = 255 - (i * 50);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		DrawBox(ROOM_LEFT - 10 + i, ROOM_TOP - 10 + i, ROOM_RIGHT + 10 - i, ROOM_BOTTOM + 10 - i, borderColor, FALSE);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// プレイ領域背景（黒いオブジェクトとのコントラストを高めるため、視認性の高いダークスレートネイビーに変更）
	DrawBox(ROOM_LEFT - 6, ROOM_TOP - 6, ROOM_RIGHT + 6, ROOM_BOTTOM + 6, ::GetColor(30, 42, 58), TRUE);
	
	// デジタルグリッド（方眼状のライン）を背景に薄く描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 40);
	unsigned int gridColor = ::GetColor(80, 110, 140);
	for (int x = ROOM_LEFT + 40; x < ROOM_RIGHT; x += 60)
	{
		DrawLine(x, ROOM_TOP, x, ROOM_BOTTOM, gridColor);
	}
	for (int y = ROOM_TOP + 40; y < ROOM_BOTTOM; y += 60)
	{
		DrawLine(ROOM_LEFT, y, ROOM_RIGHT, y, gridColor);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void MainScene::DrawEntities()
{
	unsigned int colorCyan1   = ::GetColor(0, 245, 255);
	unsigned int colorCyan2   = ::GetColor(0, 120, 200);
	unsigned int colorCyan3   = ::GetColor(220, 255, 255);

	unsigned int colorOrange1 = ::GetColor(255, 90, 0);
	unsigned int colorOrange2 = ::GetColor(255, 190, 0);
	unsigned int colorOrange3 = ::GetColor(255, 255, 100);

	unsigned int colorPurple1 = ::GetColor(170, 50, 255);
	unsigned int colorPurple2 = ::GetColor(255, 0, 150);
	unsigned int colorPurple3 = ::GetColor(240, 220, 255);

	unsigned int colorPink1   = ::GetColor(255, 130, 170);
	unsigned int colorPink2   = ::GetColor(255, 215, 80);
	unsigned int colorPink3   = ::GetColor(255, 255, 240);

	unsigned int colorTrapRed    = ::GetColor(220, 30, 30);
	unsigned int colorTrapDarkRed= ::GetColor(120, 10, 10);
	unsigned int colorTrapYellow = ::GetColor(255, 215, 0);
	unsigned int colorBlack      = ::GetColor(15, 15, 15);
	unsigned int colorWhite      = ::GetColor(255, 255, 255);

	for (int i = 0; i < MAX_FALLING_ENTITIES; ++i)
	{
		const GameEntity& e = m_entities[i];
		if (!e.active) continue;

		// 1. ドロップシャドウを描画（半透明の黒い影を少し右下にずらして描く）
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
		if (e.usesRectCollider) {
			DrawBox((int)(e.x - e.halfWidth + 6), (int)(e.y - e.halfHeight + 6), (int)(e.x + e.halfWidth + 6), (int)(e.y + e.halfHeight + 6), ::GetColor(0, 0, 0), TRUE);
		} else {
			DrawCircle((int)(e.x + 5), (int)(e.y + 5), (int)e.radius, ::GetColor(0, 0, 0), TRUE);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// 2. 各犯罪タイプ（CrimeType）に応じたおしゃれなベクターグラフィックスを描画
		switch (e.crime)
		{
		case CrimeType::Hack: // サイバーターゲット・シールド風
			{
				float angle = (float)GetNowCount() * 0.002f;
				// 同心円のアウトライン
				DrawCircle((int)e.x, (int)e.y, (int)e.radius, colorCyan2, FALSE);
				DrawCircle((int)e.x, (int)e.y, (int)(e.radius * 0.75f), colorCyan1, FALSE);
				DrawCircle((int)e.x, (int)e.y, (int)(e.radius * 0.35f), colorCyan1, TRUE);
				DrawCircle((int)e.x, (int)e.y, (int)(e.radius * 0.15f), colorCyan3, TRUE);

				// 回転する4つの照準マーク
				float tickR = e.radius * 1.0f;
				for (int k = 0; k < 4; ++k)
				{
					float tAngle = angle + k * (3.14159265f / 2.0f);
					int px = (int)(e.x + std::cos(tAngle) * tickR);
					int py = (int)(e.y + std::sin(tAngle) * tickR);
					DrawLine(px - 5, py, px + 5, py, colorCyan1);
					DrawLine(px, py - 5, px, py + 5, colorCyan1);
				}

				// 十字の目盛りライン
				DrawLine((int)(e.x - e.radius * 1.1f), (int)e.y, (int)(e.x - e.radius * 0.5f), (int)e.y, colorCyan1);
				DrawLine((int)(e.x + e.radius * 0.5f), (int)e.y, (int)(e.x + e.radius * 1.1f), (int)e.y, colorCyan1);
				DrawLine((int)e.x, (int)(e.y - e.radius * 1.1f), (int)e.x, (int)(e.y - e.radius * 0.5f), colorCyan1);
				DrawLine((int)e.x, (int)(e.y + e.radius * 0.5f), (int)e.x, (int)(e.y + e.radius * 1.1f), colorCyan1);
			}
			break;

		case CrimeType::Bomb: // 警告ギア・爆弾コア風
			{
				float angle = (float)GetNowCount() * 0.0025f;
				// 外周の細い警告円
				DrawCircle((int)e.x, (int)e.y, (int)e.radius, colorOrange1, FALSE);

				// 回転する8つのギザギザ歯車牙
				int numTeeth = 8;
				for (int k = 0; k < numTeeth; ++k)
				{
					float tAngle = angle + k * (2.0f * 3.14159265f / numTeeth);
					int x1 = (int)(e.x + std::cos(tAngle - 0.15f) * (e.radius * 0.7f));
					int y1 = (int)(e.y + std::sin(tAngle - 0.15f) * (e.radius * 0.7f));
					int x2 = (int)(e.x + std::cos(tAngle + 0.15f) * (e.radius * 0.7f));
					int y2 = (int)(e.y + std::sin(tAngle + 0.15f) * (e.radius * 0.7f));
					int x3 = (int)(e.x + std::cos(tAngle) * (e.radius * 1.05f));
					int y3 = (int)(e.y + std::sin(tAngle) * (e.radius * 1.05f));
					DrawTriangle(x1, y1, x2, y2, x3, y3, colorOrange2, TRUE);
				}

				// コア本体
				DrawCircle((int)e.x, (int)e.y, (int)(e.radius * 0.75f), colorOrange1, TRUE);
				DrawCircle((int)e.x, (int)e.y, (int)(e.radius * 0.45f), colorOrange2, TRUE);
				DrawCircle((int)e.x, (int)e.y, (int)(e.radius * 0.2f), colorOrange3, TRUE);
			}
			break;

		case CrimeType::Ritual: // 魔法陣・禁忌のオカルトマーク風
			{
				float angle = (float)GetNowCount() * -0.0015f; // 逆回転で妖しさを表現
				// 多重の魔法円
				DrawCircle((int)e.x, (int)e.y, (int)e.radius, colorPurple1, FALSE);
				DrawCircle((int)e.x, (int)e.y, (int)(e.radius * 0.94f), colorPurple1, FALSE);
				DrawCircle((int)e.x, (int)e.y, (int)(e.radius * 0.65f), colorPurple2, FALSE);

				// 回転する星型（五芒星）を描画
				int px[5], py[5];
				for (int k = 0; k < 5; ++k)
				{
					float tAngle = angle + k * (4.0f * 3.14159265f / 5.0f);
					px[k] = (int)(e.x + std::cos(tAngle) * (e.radius * 0.88f));
					py[k] = (int)(e.y + std::sin(tAngle) * (e.radius * 0.88f));
				}
				for (int k = 0; k < 5; ++k)
				{
					DrawLine(px[k], py[k], px[(k + 1) % 5], py[(k + 1) % 5], colorPurple2);
				}

				// 内部の逆回転する三角形
				int numTri = 3;
				for (int k = 0; k < numTri; ++k)
				{
					float tAngle = -angle + k * (2.0f * 3.14159265f / numTri);
					int x1 = (int)(e.x + std::cos(tAngle) * (e.radius * 0.55f));
					int y1 = (int)(e.y + std::sin(tAngle) * (e.radius * 0.55f));
					int x2 = (int)(e.x + std::cos(tAngle + 2.0f * 3.14159265f / 3.0f) * (e.radius * 0.55f));
					int y2 = (int)(e.y + std::sin(tAngle + 2.0f * 3.14159265f / 3.0f) * (e.radius * 0.55f));
					DrawLine(x1, y1, x2, y2, colorPurple1);
				}

				// 中心部
				DrawCircle((int)e.x, (int)e.y, (int)(e.radius * 0.25f), colorPurple3, TRUE);
			}
			break;

		case CrimeType::Snack: // ポップで美味しそうなキャンディ風
			{
				float angle = (float)GetNowCount() * 0.002f;
				float cosA = std::cos(angle);
				float sinA = std::sin(angle);

				// 包み紙のひねり部分（回転させた左右の三角形）
				auto drawRotatedTriangle = [&](float lx1, float ly1, float lx2, float ly2, float lx3, float ly3, unsigned int col) {
					int x1 = (int)(e.x + lx1 * cosA - ly1 * sinA);
					int y1 = (int)(e.y + lx1 * sinA + ly1 * cosA);
					int x2 = (int)(e.x + lx2 * cosA - ly2 * sinA);
					int y2 = (int)(e.y + lx2 * sinA + ly2 * cosA);
					int x3 = (int)(e.x + lx3 * cosA - ly3 * sinA);
					int y3 = (int)(e.y + lx3 * sinA + ly3 * cosA);
					DrawTriangle(x1, y1, x2, y2, x3, y3, col, TRUE);
				};

				// 左のひねり
				drawRotatedTriangle(-e.radius * 0.4f, 0.0f, -e.radius * 0.95f, -e.radius * 0.45f, -e.radius * 0.95f, e.radius * 0.45f, colorPink2);
				drawRotatedTriangle(-e.radius * 0.4f, 0.0f, -e.radius * 0.85f, -e.radius * 0.25f, -e.radius * 0.85f, e.radius * 0.25f, colorPink3);

				// 右のひねり
				drawRotatedTriangle(e.radius * 0.4f, 0.0f, e.radius * 0.95f, -e.radius * 0.45f, e.radius * 0.95f, e.radius * 0.45f, colorPink2);
				drawRotatedTriangle(e.radius * 0.4f, 0.0f, e.radius * 0.85f, -e.radius * 0.25f, e.radius * 0.85f, e.radius * 0.25f, colorPink3);

				// キャンディの球体本体
				DrawCircle((int)e.x, (int)e.y, (int)(e.radius * 0.62f), colorPink1, TRUE);

				// キャンディのうずまき模様
				int numSwirls = 4;
				for (int k = 0; k < numSwirls; ++k)
				{
					float swirlAngle = angle + k * (2.0f * 3.14159265f / numSwirls);
					int xStart = (int)e.x;
					int yStart = (int)e.y;
					int xEnd = (int)(e.x + std::cos(swirlAngle) * (e.radius * 0.55f));
					int yEnd = (int)(e.y + std::sin(swirlAngle) * (e.radius * 0.55f));
					DrawLine(xStart, yStart, xEnd, yEnd, colorPink3);
				}

				// ハイライト
				DrawCircle((int)(e.x - e.radius * 0.2f), (int)(e.y - e.radius * 0.2f), (int)(e.radius * 0.15f), colorPink3, TRUE);
			}
			break;

		case CrimeType::Trap: // 警告用のトゲトゲ・危険トラップ風（矩形）
			{
				int xMin = (int)(e.x - e.halfWidth);
				int xMax = (int)(e.x + e.halfWidth);
				int yMin = (int)(e.y - e.halfHeight);
				int yMax = (int)(e.y + e.halfHeight);

				// 上下のトゲトゲを描画
				int spikeSize = 8;
				for (int px = xMin + 10; px <= xMax - 10; px += 16)
				{
					DrawTriangle(px - 6, yMin, px + 6, yMin, px, yMin - spikeSize, colorTrapDarkRed, TRUE);
					DrawTriangle(px - 4, yMin, px + 4, yMin, px, yMin - spikeSize + 2, colorTrapRed, TRUE);
				}
				for (int px = xMin + 10; px <= xMax - 10; px += 16)
				{
					DrawTriangle(px - 6, yMax, px + 6, yMax, px, yMax + spikeSize, colorTrapDarkRed, TRUE);
					DrawTriangle(px - 4, yMax, px + 4, yMax, px, yMax + spikeSize - 2, colorTrapRed, TRUE);
				}

				// 1. 黄色の背景塗りつぶし
				DrawBox(xMin + 6, yMin + 6, xMax - 6, yMax - 6, colorTrapYellow, TRUE);

				// 2. 黒い斜めゼブラ線を背景の上に描画（はみ出して描画して後から枠で隠す）
				for (int offset = -80; offset <= 80; offset += 16)
				{
					for (int w = -3; w <= 3; ++w)
					{
						DrawLine(xMin + offset + w, yMin + 6, xMin + offset + (yMax - yMin - 12) + w, yMax - 6, colorBlack);
					}
				}

				// 3. 枠をゼブラ線の上から重ねて描く（マスク処理の代わり）
				// 外周の黒枠
				DrawBox(xMin, yMin, xMin + 6, yMax, colorBlack, TRUE); // 左枠
				DrawBox(xMax - 6, yMin, xMax, yMax, colorBlack, TRUE); // 右枠
				DrawBox(xMin, yMin, xMax, yMin + 6, colorBlack, TRUE); // 上枠
				DrawBox(xMin, yMax - 6, xMax, yMax, colorBlack, TRUE); // 下枠

				// 内側の赤枠
				DrawBox(xMin + 3, yMin + 3, xMin + 6, yMax - 3, colorTrapRed, TRUE); // 左内赤
				DrawBox(xMax - 6, yMin + 3, xMax - 3, yMax - 3, colorTrapRed, TRUE); // 右内赤
				DrawBox(xMin + 3, yMin + 3, xMax - 3, yMin + 6, colorTrapRed, TRUE); // 上内赤
				DrawBox(xMin + 3, yMax - 6, xMax - 3, yMax - 3, colorTrapRed, TRUE); // 下内赤

				// 4. 中央の「！」マークを描画
				int innerHeight = (int)(e.halfHeight * 0.9f);
				DrawBox((int)e.x - 3, (int)e.y - innerHeight + 6, (int)e.x + 3, (int)e.y + 2, colorBlack, TRUE);
				DrawCircle((int)e.x, (int)(e.y + innerHeight - 8), 4, colorBlack, TRUE);

				// マーク内ハイライト
				DrawBox((int)e.x - 1, (int)e.y - innerHeight + 8, (int)e.x + 1, (int)e.y - 2, colorWhite, TRUE);
			}
			break;
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
	// 1. タイマー枠（左上：水色ガラスパネル）
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 90);
		DrawRoundRect(10, 10, 200, 60, 8, 8, ::GetColor(10, 20, 35), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		
		DrawRoundRect(10, 10, 200, 60, 8, 8, ::GetColor(0, 160, 255), FALSE);
		SetFontSize(20);
		// タイムアップ間近で点滅
		unsigned int timerCol = (m_timeLeftSec <= 10 && GetNowCount() % 500 < 250) ? ::GetColor(255, 50, 50) : ::GetColor(220, 240, 255);
		DrawFormatString(28, 24, timerCol, STR_MAIN_TIMER, m_timeLeftSec);
	}

	// 2. カオスポイント・スコア枠（中央左：ゴールドガラスパネル）
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 90);
		DrawRoundRect(220, 10, 560, 60, 8, 8, ::GetColor(15, 20, 30), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		
		DrawRoundRect(220, 10, 560, 60, 8, 8, ::GetColor(255, 190, 40), FALSE);
		SetFontSize(20);
		DrawFormatString(240, 24, ::GetColor(255, 225, 160), STR_MAIN_SCORE, m_score);
	}

	// 3. 難易度枠（右上：エメラルドガラスパネル）
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 90);
		DrawRoundRect(1070, 10, 1270, 60, 8, 8, ::GetColor(10, 25, 20), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		
		DrawRoundRect(1070, 10, 1270, 60, 8, 8, ::GetColor(40, 220, 110), FALSE);
		SetFontSize(20);
		GameDifficulty diff = GameSession::GetDifficulty();
		const wchar_t* diffStr = (diff == GameDifficulty::Easy) ? STR_DIFF_EASY : (diff == GameDifficulty::Hell) ? STR_DIFF_HELL : STR_DIFF_NORMAL;
		DrawFormatString(1105, 24, ::GetColor(180, 255, 190), L"%s", diffStr);
	}

	// 4. コンボ枠（左側タイマーの下：オレンジガラスパネル）
	if (m_combo >= 2)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 110);
		DrawRoundRect(10, 80, 200, 130, 8, 8, ::GetColor(25, 15, 10), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		
		DrawRoundRect(10, 80, 200, 130, 8, 8, ::GetColor(255, 110, 30), FALSE);
		SetFontSize(24);
		DrawFormatString(26, 92, ::GetColor(255, 165, 80), STR_MAIN_COMBO, m_combo);
		
		// 進捗バー（コンボ維持時間）
		int barW = (int)((float)m_comboTimer / (TARGET_FPS * 2) * 160.0f);
		DrawBox(20, 114, 180, 120, ::GetColor(40, 25, 15), TRUE);
		DrawBox(20, 114, 20 + barW, 120, ::GetColor(255, 130, 40), TRUE);
	}

	// 5. フィーバータイム警告表示（中央：ピンクのネオンフラッシュパネル）
	if (m_feverTimer > 0)
	{
		int flashAlpha = (int)(150 + std::sin((float)GetNowCount() * 0.012f) * 105);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, flashAlpha);
		DrawRoundRect(580, 10, 1050, 60, 8, 8, ::GetColor(255, 30, 120), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		
		DrawRoundRect(580, 10, 1050, 60, 8, 8, ::GetColor(255, 220, 240), FALSE);
		SetFontSize(22);
		DrawFormatString(735, 23, ::GetColor(255, 255, 255), STR_MAIN_FEVER);
	}

	// 6. 下部インフォメーションコントロールパネル（ゲーム説明とSFテキスト ticker）
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 90);
		DrawRoundRect(220, 634, 1060, 710, 8, 8, ::GetColor(12, 18, 28), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		
		DrawRoundRect(220, 634, 1060, 710, 8, 8, ::GetColor(0, 120, 180), FALSE);
		
		SetFontSize(16);
		// ヒントテキスト
		DrawString(240, 646, STR_MAIN_HINT, ::GetColor(150, 210, 255));
		
		// システムログ風のダミーテキスト（時間で少し動くように見せる）
		unsigned int logColor = ::GetColor(90, 140, 180);
		int dotCount = (GetNowCount() / 400) % 4;
		wchar_t dots[5] = L"";
		for (int d = 0; d < dotCount; ++d) dots[d] = L'.';
		dots[dotCount] = L'\0';
		
		if (m_feverTimer > 0) {
			DrawFormatString(240, 676, ::GetColor(255, 60, 150), L"FEVER INTRUSION DETECTED // MULTIPLIER x2 ACTIVE // POWER BOOST ON%s", dots);
		} else if (m_timeLeftSec <= 10) {
			DrawFormatString(240, 676, ::GetColor(255, 80, 80), L"CRITICAL WARNING // TEMPORAL ANOMALY DETECTED // HASTEN EFFORTS%s", dots);
		} else {
			DrawFormatString(240, 676, logColor, L"SECURE MONITORING LAB L2 // SCANNING ATMOSPHERIC RESONANCE%s", dots);
		}
	}
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

SceneID MainScene::GetNextSceneID() const
{
	if (m_requestGoTitle)  return SceneID::Title;
	if (m_requestGoResult)
	{
		LoadingScene::SetTargetScene(SceneID::Result);
		return SceneID::Loading;
	}
	return SceneID::None;
}

// =============================================================================
// ポーズメニュー
// =============================================================================

void MainScene::UpdatePauseMenu()
{
	// クリック判定フレーム更新（ポーズ中はここで行う）
	UiMouse::UpdateFrame();
	const bool escNow = (CheckHitKey(KEY_INPUT_ESCAPE) != 0);
	const bool escPressed = escNow && !m_prevEscKey;
	m_prevEscKey = escNow;

	switch (m_pauseState)
	{
	case PauseState::None:
		// ESCキーでポーズを開く
		if (escPressed)
		{
			m_pauseState = PauseState::Menu;
			m_prevMouseInput = GetMouseInput(); // クリック誤判定防止
		}
		break;

	case PauseState::Menu:
	{
		// ボタン座標（DrawPauseOverlayと一致させること）
		const int CX = SCREEN_WIDTH / 2;
		const int BTN_W = 320;
		const int BTN_H = 56;
		const int BTN_X1 = CX - BTN_W / 2;
		const int BTN_X2 = CX + BTN_W / 2;
		const int Y_RESUME  = 300;
		const int Y_SETTING = 380;
		const int Y_TITLE   = 460;

		if (UiMouse::TryClick(BTN_X1, Y_RESUME,  BTN_X2, Y_RESUME  + BTN_H))
		{
			m_pauseState = PauseState::None;
		}
		else if (UiMouse::TryClick(BTN_X1, Y_SETTING, BTN_X2, Y_SETTING + BTN_H))
		{
			m_pauseState     = PauseState::Settings;
			m_igSensValue    = GameSession::GetMouseSensitivity();
			m_igCurValue     = GameSession::GetCursorRadius();
			m_igSensDragging = false;
			m_igCurDragging  = false;
		}
		else if (UiMouse::TryClick(BTN_X1, Y_TITLE, BTN_X2, Y_TITLE + BTN_H))
		{
			m_requestGoTitle = true;
		}
		break;
	}

	case PauseState::Settings:
		UpdateInGameSettings();
		break;
	}
}

void MainScene::DrawPauseOverlay()
{
	// 半透明の暗いオーバーレイ
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
	DrawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ::GetColor(5, 8, 15), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	const int CX = SCREEN_WIDTH / 2;
	const int BTN_W = 320;
	const int BTN_H = 56;
	const int BTN_X1 = CX - BTN_W / 2;
	const int BTN_X2 = CX + BTN_W / 2;
	const int Y_RESUME  = 300;
	const int Y_SETTING = 380;
	const int Y_TITLE   = 460;

	// パネル枠
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 210);
	DrawRoundRect(CX - 220, 220, CX + 220, 560, 16, 16, ::GetColor(12, 20, 35), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawRoundRect(CX - 220, 220, CX + 220, 560, 16, 16, ::GetColor(0, 160, 255), FALSE);
	DrawRoundRect(CX - 218, 222, CX + 218, 558, 15, 15, ::GetColor(0, 80, 140), FALSE);

	// タイトル「PAUSE」
	SetFontSize(40);
	DrawString(CX - 72, 240, L"PAUSE", ::GetColor(0, 220, 255));

	// 区切りライン
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
	DrawLine(CX - 160, 292, CX + 160, 292, ::GetColor(0, 120, 200));
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// マウス座標（ホバー判定用）
	int mx, my; GetMousePoint(&mx, &my);

	// ボタン描画ラムダ
	auto drawBtn = [&](int y, const wchar_t* label, unsigned int baseCol, unsigned int hoverCol)
	{
		const bool hovered = (mx >= BTN_X1 && mx <= BTN_X2 && my >= y && my <= y + BTN_H);
		unsigned int col = hovered ? hoverCol : baseCol;
		int alpha = hovered ? 220 : 160;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		DrawRoundRect(BTN_X1, y, BTN_X2, y + BTN_H, 10, 10, col, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawRoundRect(BTN_X1, y, BTN_X2, y + BTN_H, 10, 10, col, FALSE);
		SetFontSize(26);
		// ラベル幅を概算して中央揃え
		int textW = (int)wcslen(label) * 20;
		DrawString(CX - textW / 2, y + (BTN_H - 26) / 2, label, ::GetColor(240, 248, 255));
	};

	drawBtn(Y_RESUME,  L"\u518d\u958b",     ::GetColor(0, 140, 220),  ::GetColor(0, 200, 255));
	drawBtn(Y_SETTING, L"\u8a2d\u5b9a",     ::GetColor(60, 90, 160),   ::GetColor(80, 130, 255));
	drawBtn(Y_TITLE,   L"\u30bf\u30a4\u30c8\u30eb", ::GetColor(120, 50, 50), ::GetColor(200, 70, 70));
}

// =============================================================================
// ゲーム中設定（ポーズ中のスライダーパネル）
// =============================================================================

void MainScene::UpdateInGameSettings()
{
	// 感度・カーソルサイズのスライダー
	const int SX1 = 360, SX2 = 920;
	const int SW  = SX2 - SX1;
	const int SENS_Y = 350;
	const int CUR_Y  = 460;
	const float SENS_MIN = 0.1f, SENS_MAX = 3.0f;
	const float CUR_MIN  = 4.0f, CUR_MAX  = 60.0f;

	int mx, my; GetMousePoint(&mx, &my);
	const bool leftDown = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

	// 感度スライダー
	if (leftDown)
	{
		if (!m_igSensDragging)
		{
			if (mx >= SX1 - 10 && mx <= SX2 + 10 && my >= SENS_Y - 20 && my <= SENS_Y + 20)
				m_igSensDragging = true;
		}
		if (m_igSensDragging)
		{
			float r = static_cast<float>(mx - SX1) / SW;
			if (r < 0.0f) r = 0.0f; if (r > 1.0f) r = 1.0f;
			m_igSensValue = SENS_MIN + r * (SENS_MAX - SENS_MIN);
			GameSession::SetMouseSensitivity(m_igSensValue);
		}
	}
	else { m_igSensDragging = false; }

	// 感度 ± ボタン
	if (UiMouse::TryClick(SX1 - 50, SENS_Y - 20, SX1 - 10, SENS_Y + 20))
	{
		m_igSensValue -= 0.05f;
		if (m_igSensValue < SENS_MIN) m_igSensValue = SENS_MIN;
		GameSession::SetMouseSensitivity(m_igSensValue);
	}
	if (UiMouse::TryClick(SX2 + 10, SENS_Y - 20, SX2 + 50, SENS_Y + 20))
	{
		m_igSensValue += 0.05f;
		if (m_igSensValue > SENS_MAX) m_igSensValue = SENS_MAX;
		GameSession::SetMouseSensitivity(m_igSensValue);
	}

	// カーソルスライダー
	if (leftDown)
	{
		if (!m_igCurDragging)
		{
			if (mx >= SX1 - 10 && mx <= SX2 + 10 && my >= CUR_Y - 20 && my <= CUR_Y + 20)
				m_igCurDragging = true;
		}
		if (m_igCurDragging)
		{
			float r = static_cast<float>(mx - SX1) / SW;
			if (r < 0.0f) r = 0.0f; if (r > 1.0f) r = 1.0f;
			m_igCurValue = CUR_MIN + r * (CUR_MAX - CUR_MIN);
			GameSession::SetCursorRadius(m_igCurValue);
		}
	}
	else { m_igCurDragging = false; }

	// カーソルサイズ ± ボタン
	if (UiMouse::TryClick(SX1 - 50, CUR_Y - 20, SX1 - 10, CUR_Y + 20))
	{
		m_igCurValue -= 2.0f;
		if (m_igCurValue < CUR_MIN) m_igCurValue = CUR_MIN;
		GameSession::SetCursorRadius(m_igCurValue);
	}
	if (UiMouse::TryClick(SX2 + 10, CUR_Y - 20, SX2 + 50, CUR_Y + 20))
	{
		m_igCurValue += 2.0f;
		if (m_igCurValue > CUR_MAX) m_igCurValue = CUR_MAX;
		GameSession::SetCursorRadius(m_igCurValue);
	}

	// 「保存して戻る」ボタン
	const int CX = SCREEN_WIDTH / 2;
	if (UiMouse::TryClick(CX - 160, 540, CX + 160, 590))
	{
		GameSession::SaveConfig();
		m_pauseState = PauseState::Menu;
	}
}

void MainScene::DrawInGameSettings()
{
	// 半透明オーバーレイ
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 190);
	DrawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ::GetColor(5, 8, 15), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	const int CX = SCREEN_WIDTH / 2;

	// パネル
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
	DrawRoundRect(200, 150, SCREEN_WIDTH - 200, 620, 16, 16, ::GetColor(12, 20, 35), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawRoundRect(200, 150, SCREEN_WIDTH - 200, 620, 16, 16, ::GetColor(80, 130, 255), FALSE);

	SetFontSize(36);
	DrawString(CX - 60, 170, L"\u8a2d\u5b9a", ::GetColor(160, 200, 255));

	const int SX1 = 360, SX2 = 920, SW = SX2 - SX1;
	const int SENS_Y = 350;
	const int CUR_Y  = 460;
	const float SENS_MIN = 0.1f, SENS_MAX = 3.0f;
	const float CUR_MIN  = 4.0f, CUR_MAX  = 60.0f;

	// 感度スライダー
	SetFontSize(22);
	DrawFormatString(CX - 180, 290, ::GetColor(220, 230, 255), L"\u30de\u30a6\u30b9\u611f\u5ea6: %.2f", m_igSensValue);
	{
		int knobX = SX1 + static_cast<int>(SW * ((m_igSensValue - SENS_MIN) / (SENS_MAX - SENS_MIN)));
		DrawBox(SX1, SENS_Y - 4, SX2, SENS_Y + 4, ::GetColor(50, 60, 80), TRUE);
		DrawBox(SX1, SENS_Y - 4, knobX, SENS_Y + 4, ::GetColor(80, 180, 255), TRUE);
		DrawCircle(knobX, SENS_Y, 13, ::GetColor(255, 210, 80), TRUE);
		DrawCircle(knobX, SENS_Y, 13, ::GetColor(255, 255, 255), FALSE);
		// ± ボタン
		UiMouse::DrawButton(SX1 - 50, SENS_Y - 20, SX1 - 10, SENS_Y + 20, L"-");
		UiMouse::DrawButton(SX2 + 10, SENS_Y - 20, SX2 + 50, SENS_Y + 20, L"+");
	}

	// カーソルサイズスライダー
	DrawFormatString(CX - 200, 400, ::GetColor(220, 230, 255), L"\u30ab\u30fc\u30bd\u30eb\u534a\u5f84: %.0f px", m_igCurValue);
	{
		int knobX = SX1 + static_cast<int>(SW * ((m_igCurValue - CUR_MIN) / (CUR_MAX - CUR_MIN)));
		DrawBox(SX1, CUR_Y - 4, SX2, CUR_Y + 4, ::GetColor(50, 60, 80), TRUE);
		DrawBox(SX1, CUR_Y - 4, knobX, CUR_Y + 4, ::GetColor(100, 255, 160), TRUE);
		DrawCircle(knobX, CUR_Y, 13, ::GetColor(100, 255, 160), TRUE);
		DrawCircle(knobX, CUR_Y, 13, ::GetColor(255, 255, 255), FALSE);
		// ± ボタン
		UiMouse::DrawButton(SX1 - 50, CUR_Y - 20, SX1 - 10, CUR_Y + 20, L"-");
		UiMouse::DrawButton(SX2 + 10, CUR_Y - 20, SX2 + 50, CUR_Y + 20, L"+");
	}

	// カーソルサイズのプレビュー
	{
		const int previewX = CX + 250;
		const int previewY = CUR_Y + 90; // moved further down to avoid UI overlap
		SetFontSize(16);
		DrawFormatString(previewX - 30, previewY - 26, ::GetColor(160, 180, 220), L"\u30d7\u30ec\u30d3\u30e5\u30fc");
		const int pr = static_cast<int>(m_igCurValue);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 55);
		DrawCircle(previewX, previewY, pr, ::GetColor(100, 200, 255), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawCircle(previewX, previewY, pr, ::GetColor(80, 180, 255), FALSE);
		DrawCircle(previewX, previewY, 5, ::GetColor(255, 255, 255), TRUE);
	}

	// 保存ボタン
	const bool hovered = UiMouse::IsOver(CX - 160, 540, CX + 160, 590);
	unsigned int btnCol = hovered ? ::GetColor(0, 200, 120) : ::GetColor(0, 140, 90);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, hovered ? 220 : 160);
	DrawRoundRect(CX - 160, 540, CX + 160, 590, 10, 10, btnCol, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawRoundRect(CX - 160, 540, CX + 160, 590, 10, 10, btnCol, FALSE);
	SetFontSize(22);
	DrawString(CX - 110, 553, L"\u4fdd\u5b58\u3057\u3066\u30e1\u30cb\u30e5\u30fc\u306b\u623b\u308b", ::GetColor(240, 255, 248));

	// ESCヒント removed
}