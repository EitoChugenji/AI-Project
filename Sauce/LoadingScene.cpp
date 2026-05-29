#include "LoadingScene.h"

#include "GameConfig.h"
#include "GameStrings.h"
#include "GameSession.h"
#include "UiMouse.h"
#include "DxLib.h"
#include <cmath>

SceneID LoadingScene::s_targetScene = SceneID::None;

// メインロード画面での表示用に簡略化された1行形式のヒントリスト
static const wchar_t* s_tips[] =
{
    L"コンボを 5 以上繋げるとフィーバー突入！スコアが 2倍 になるぞ！",
    L"赤いトゲトゲは罠！クリックするとスコアと残り時間が減ってしまうぞ！",
    L"的を画面外に落とすとスコアが減り、コンボも途切れるので注意しよう！",
    L"コンボを繋ぐほどスコアの倍率がアップ！コンボ維持を目指そう！"
};
static const int TIP_COUNT = sizeof(s_tips) / sizeof(s_tips[0]);

void LoadingScene::Init()
{
    m_frameCounter = 0;
    m_maxFrames    = 120; // 2 seconds at 60fps
    m_tipIndex     = GetRand(TIP_COUNT - 1);
}

void LoadingScene::Update()
{
    UiMouse::UpdateFrame();
    m_frameCounter++;
}

void LoadingScene::Draw()
{
    // 1. Space gradient background
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        DrawLine(0, y, SCREEN_WIDTH, y, ::GetColor(8, 12, 18 + (y * 50 / SCREEN_HEIGHT)));
    }

    // 2. Twinkling stars
    for (int i = 0; i < 60; ++i)
    {
        const int sx = (i * 173) % SCREEN_WIDTH;
        const int sy = (i * 97)  % SCREEN_HEIGHT;
        const int br = 140 + (int)(115.0f * std::sin((float)GetNowCount() * 0.0018f + i * 1.5f));
        DrawCircle(sx, sy, 1 + (i % 2), ::GetColor(br, br, br + 15), TRUE);
    }

    // 3. HUD brackets
    unsigned int hudColor   = ::GetColor(0, 160, 255);
    const int    bracketLen = 20;
    DrawLine(5, 5, 5 + bracketLen, 5, hudColor);
    DrawLine(5, 5, 5, 5 + bracketLen, hudColor);
    DrawLine(SCREEN_WIDTH - 5, 5, SCREEN_WIDTH - 5 - bracketLen, 5, hudColor);
    DrawLine(SCREEN_WIDTH - 5, 5, SCREEN_WIDTH - 5, 5 + bracketLen, hudColor);
    DrawLine(5, SCREEN_HEIGHT - 5, 5 + bracketLen, SCREEN_HEIGHT - 5, hudColor);
    DrawLine(5, SCREEN_HEIGHT - 5, 5, SCREEN_HEIGHT - 5 - bracketLen, hudColor);
    DrawLine(SCREEN_WIDTH - 5, SCREEN_HEIGHT - 5, SCREEN_WIDTH - 5 - bracketLen, SCREEN_HEIGHT - 5, hudColor);
    DrawLine(SCREEN_WIDTH - 5, SCREEN_HEIGHT - 5, SCREEN_WIDTH - 5, SCREEN_HEIGHT - 5 - bracketLen, hudColor);

    // 4. Centre glassmorphism container
    const int boxL = 240, boxR = 1040, boxT = 150, boxB = 570;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
    DrawBox(boxL, boxT, boxR, boxB, ::GetColor(18, 28, 44), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    for (int i = 0; i < 4; ++i)
    {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - i * 50);
        DrawBox(boxL - 4 + i, boxT - 4 + i, boxR + 4 - i, boxB + 4 - i, hudColor, FALSE);
    }
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 30);
    unsigned int gridCol = ::GetColor(80, 110, 140);
    for (int gx = boxL + 40; gx < boxR; gx += 60) DrawLine(gx, boxT, gx, boxB, gridCol);
    for (int gy = boxT + 40; gy < boxB; gy += 60) DrawLine(boxL, gy, boxR, gy, gridCol);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // 遷移先シーンに応じたテキスト表示（ゲーム開始前はヒント、リザルト前は解析ログ）
    if (s_targetScene == SceneID::Main)
    {
        // ---- PRE-GAME HINT ----
        SetFontSize(22);
        int headerW = GetDrawStringWidth(L"GAME TIP", -1);
        DrawFormatString(SCREEN_WIDTH / 2 - headerW / 2, boxT + 60, ::GetColor(150, 180, 255), L"GAME TIP");

        DrawLine(boxL + 100, boxT + 110, boxR - 100, boxT + 110, ::GetColor(0, 120, 200));

        const wchar_t* tip = s_tips[m_tipIndex];
        SetFontSize(26);
        int tipW = GetDrawStringWidth(tip, -1);
        DrawFormatString(SCREEN_WIDTH / 2 - tipW / 2, boxT + 220, ::GetColor(255, 255, 255), L"%s", tip);
    }
    else
    {
        // ---- PRE-RESULT ANALYSIS ----
        SetFontSize(28);
        DrawFormatString(boxL + 50, boxT + 40, ::GetColor(230, 150, 255),
            L"セッション終了  ―  データ解析中");

        SetFontSize(22);
        DrawFormatString(boxL + 60, boxT + 120, ::GetColor(170, 190, 220),
            L"システムシールド  :  非アクティブ化中...");
        DrawFormatString(boxL + 60, boxT + 160, ::GetColor(170, 190, 220),
            L"オーラエネルギー  :  シャットダウン");

        // progressive log
        if (m_frameCounter < 40)
        {
            DrawFormatString(boxL + 60, boxT + 230, ::GetColor(0, 255, 130),
                L"▶  アクティビティログをダンプ中...");
        }
        else if (m_frameCounter < 80)
        {
            DrawFormatString(boxL + 60, boxT + 230, ::GetColor(0, 255, 130),
                L"▶  アクティビティログをダンプ中...  完了");
            DrawFormatString(boxL + 60, boxT + 268, ::GetColor(0, 255, 130),
                L"▶  スコア・コンボデータを照合中...");
        }
        else
        {
            DrawFormatString(boxL + 60, boxT + 230, ::GetColor(0, 255, 130),
                L"▶  アクティビティログをダンプ中...  完了");
            DrawFormatString(boxL + 60, boxT + 268, ::GetColor(0, 255, 130),
                L"▶  スコア・コンボデータを照合中...  完了");
            DrawFormatString(boxL + 60, boxT + 306, ::GetColor(255, 235, 100),
                L"▶  スコアシートの出力待機中...");
        }
    }

    // 6. Progress bar
    const int barX = 340, barY = boxB - 100, barW = 600, barH = 22;
    float ratio   = (float)m_frameCounter / (float)m_maxFrames;
    if (ratio > 1.0f) ratio = 1.0f;
    int currentW  = (int)(barW * ratio);

    DrawBox(barX, barY, barX + barW, barY + barH, ::GetColor(20, 35, 50), TRUE);
    DrawBox(barX, barY, barX + barW, barY + barH, ::GetColor(0, 100, 180), FALSE);

    unsigned int barColor = (s_targetScene == SceneID::Main) ? ::GetColor(0, 245, 255) : ::GetColor(170, 50, 255);
    if (currentW > 4)
    {
        DrawBox(barX + 2, barY + 2, barX + currentW - 2, barY + barH - 2, barColor, TRUE);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
        DrawBox(barX, barY - 2, barX + currentW, barY + barH + 2, barColor, FALSE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    SetFontSize(18);
    int pct = (int)(ratio * 100.0f);
    wchar_t pctStr[64];
    if (pct < 100)
        swprintf_s(pctStr, L"CONNECTING SYSTEM... %d%%", pct);
    else
        swprintf_s(pctStr, L"SYSTEM ONLINE  100%%");
    int strW = GetDrawStringWidth(pctStr, -1);
    DrawFormatString(SCREEN_WIDTH / 2 - strW / 2, barY - 30, ::GetColor(220, 240, 255), L"%s", pctStr);

    UiMouse::DrawCursor(GameSession::GetCursorRadius(), false);
}

SceneID LoadingScene::GetNextSceneID() const
{
    if (m_frameCounter >= m_maxFrames)
        return s_targetScene;
    return SceneID::None;
}

void LoadingScene::SetTargetScene(SceneID target)
{
    s_targetScene = target;
}