#pragma once

// =============================================================================
// GameConfig.h … ゲーム全体の設定値
// =============================================================================

static const int SCREEN_WIDTH = 1280;
static const int SCREEN_HEIGHT = 720;
static const int TARGET_FPS = 60;

static const int GAME_DURATION_SEC = 30;
static const int RUSH_WARNING_SEC = 10;

// クリックしないとゲームオーバーになる時間（秒）
static const int IDLE_GAMEOVER_SEC = 8;

// フィーバータイム（COMBO がこの値以上で発動）
static const int FEVER_COMBO_THRESHOLD = 5;
static const int FEVER_DURATION_SEC = 6;

// プレイエリア（落下オブジェクトの範囲）
static const int ROOM_LEFT = 220;
static const int ROOM_TOP = 140;
static const int ROOM_RIGHT = 1060;
static const int ROOM_BOTTOM = 620;

// 落下オブジェクト
static const int MAX_FALLING_ENTITIES = 32;
static const int MAX_POPUP_TEXTS = 12;
static const int MAX_STARS = 80;

static const float FALL_SPEED_BASE = 4.8f;
static const float FALL_SPEED_MAX = 10.5f;
static const int SPAWN_INTERVAL_MAX_FRAMES = 26;
static const int SPAWN_INTERVAL_MIN_FRAMES = 7;
static const int MAX_ON_SCREEN_MIN = 7;
static const int MAX_ON_SCREEN_MAX = 18;
static const int OBSTACLE_SPAWN_PERCENT = 22;

// 妨害オブジェクト（クリック時ペナルティ）
static const int OBSTACLE_PENALTY_SCORE = 80;
static const int OBSTACLE_PENALTY_TIME_SEC = 3;

// 取り逃し（画面下で消えたとき・クリックしなかった場合）
static const int MISS_ANIMAL_PENALTY_SCORE = 50;
static const int MISS_ITEM_PENALTY_SCORE = 25;

// ランキング
static const int RANKING_NAME_MAX_LEN = 10;
static const int RANKING_MAX_ENTRIES = 10;
static const wchar_t RANKING_FILE_NAME_NORMAL[] = L"ranking_normal.dat";
static const wchar_t RANKING_FILE_NAME_EASY[] = L"ranking_easy.dat";
