#pragma once

// ウィンドウ・フレームレート
static const int SCREEN_WIDTH  = 1280;
static const int SCREEN_HEIGHT = 720;
static const int TARGET_FPS    = 60;

// ゲーム時間（秒）
static const int GAME_DURATION_SEC  = 30;
// 残り時間がこの値以下になると RUSH 演出に切り替わる
static const int RUSH_WARNING_SEC   = 10;

// 一定時間クリックしないとゲームオーバーになる無操作タイムアウト
static const int IDLE_GAMEOVER_SEC  = 8;

// フィーバー発動条件（COMBOがこの値以上で発動）とフィーバー継続時間
static const int FEVER_COMBO_THRESHOLD = 5;
static const int FEVER_DURATION_SEC    = 6;

// 落下オブジェクトが出現・移動するプレイエリアの矩形（ピクセル）
static const int ROOM_LEFT   = 220;
static const int ROOM_TOP    = 140;
static const int ROOM_RIGHT  = 1060;
static const int ROOM_BOTTOM = 620;

// 同時存在できる落下オブジェクト・ポップアップ・星の上限数
static const int MAX_FALLING_ENTITIES = 32;
static const int MAX_POPUP_TEXTS      = 12;
static const int MAX_STARS            = 80;

// 落下速度（ゲーム経過に応じて BASE から MAX に線形補間）
static const float FALL_SPEED_BASE = 4.8f;
static const float FALL_SPEED_MAX  = 10.5f;

// オブジェクト出現間隔フレーム数（時間経過で MAX から MIN に短縮）
static const int SPAWN_INTERVAL_MAX_FRAMES = 26;
static const int SPAWN_INTERVAL_MIN_FRAMES = 7;

// 同時表示数の範囲（時間経過で MIN から MAX に増加）
static const int MAX_ON_SCREEN_MIN = 7;
static const int MAX_ON_SCREEN_MAX = 18;

// 妨害オブジェクト（トラップ）の出現確率（%）
static const int OBSTACLE_SPAWN_PERCENT = 22;

// トラップをクリックしたときのペナルティ
static const int OBSTACLE_PENALTY_SCORE    = 80;
static const int OBSTACLE_PENALTY_TIME_SEC = 3;

// オブジェクトを取り逃した（画面外に落下）ときのスコアペナルティ
static const int MISS_ANIMAL_PENALTY_SCORE = 50;
static const int MISS_ITEM_PENALTY_SCORE   = 25;

// ランキング設定
static const int      RANKING_NAME_MAX_LEN            = 10;
static const int      RANKING_MAX_ENTRIES              = 10;
static const wchar_t  RANKING_FILE_NAME_NORMAL[] = L"ranking_normal.dat";
static const wchar_t  RANKING_FILE_NAME_EASY[]   = L"ranking_easy.dat";
static const wchar_t  RANKING_FILE_NAME_HELL[]   = L"ranking_hell.dat";