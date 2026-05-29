#include "RankingScene.h"

#include "GameConfig.h"
#include "GameStrings.h"
#include "GameSession.h"
#include "RankingManager.h"
#include "UiMouse.h"
#include "DxLib.h"
#include <cmath>

namespace
{
	static const int BTN_LEFT = 440;
	static const int BTN_RIGHT = 840;
	static const int BTN_BACK_TOP = 640;
	static const int BTN_BACK_BOTTOM = 700;
	static const int BTN_CLEAR_LEFT = 1000;
	static const int BTN_CLEAR_RIGHT = 1220;
	static const int BTN_CLEAR_TOP = 40;
	static const int BTN_CLEAR_BOTTOM = 90;
	static const int BTN_TAB_EASY_LEFT = 320;
	static const int BTN_TAB_EASY_RIGHT = 520;
	static const int BTN_TAB_NORMAL_LEFT = 540;
	static const int BTN_TAB_NORMAL_RIGHT = 740;
	static const int BTN_TAB_HELL_LEFT = 760;
	static const int BTN_TAB_HELL_RIGHT = 960;
	static const int BTN_TAB_TOP = 100;
	static const int BTN_TAB_BOTTOM = 140;
}

void RankingScene::Init()
{
	m_requestReturn = false;
	// 別シーンでランキングが更新されている可能性があるため起動時に再読み込みする
	RankingManager::Load();
}

void RankingScene::Update()
{
	UiMouse::UpdateFrame();

	if (UiMouse::TryClick(BTN_LEFT, BTN_BACK_TOP, BTN_RIGHT, BTN_BACK_BOTTOM))
	{
		m_requestReturn = true;
	}
	
	if (UiMouse::TryClick(BTN_CLEAR_LEFT, BTN_CLEAR_TOP, BTN_CLEAR_RIGHT, BTN_CLEAR_BOTTOM))
	{
		RankingManager::ClearAll();
	}
	
	if (UiMouse::TryClick(BTN_TAB_EASY_LEFT, BTN_TAB_TOP, BTN_TAB_EASY_RIGHT, BTN_TAB_BOTTOM))
	{
		RankingManager::SetViewDifficulty(GameDifficulty::Easy);
	}
	if (UiMouse::TryClick(BTN_TAB_NORMAL_LEFT, BTN_TAB_TOP, BTN_TAB_NORMAL_RIGHT, BTN_TAB_BOTTOM))
	{
		RankingManager::SetViewDifficulty(GameDifficulty::Normal);
	}
	if (UiMouse::TryClick(BTN_TAB_HELL_LEFT, BTN_TAB_TOP, BTN_TAB_HELL_RIGHT, BTN_TAB_BOTTOM))
	{
		RankingManager::SetViewDifficulty(GameDifficulty::Hell);
	}
}

void RankingScene::Draw()
{
	// 宇宙グラデーション背景
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		DrawLine(0, y, SCREEN_WIDTH, y, ::GetColor(8, 12, 18 + (y * 50 / SCREEN_HEIGHT)));
	}

	// インデックスごとに固定位置・位相をずらして点滅させる（毎フレームrandを使わず決定論的に描画）
	for (int i = 0; i < 60; ++i)
	{
		const int x = (i * 173) % SCREEN_WIDTH;
		const int y = (i * 97) % SCREEN_HEIGHT;
		const int brightness = 140 + (int)(115.0f * std::sin((float)GetNowCount() * 0.0018f + i * 1.5f));
		DrawCircle(x, y, 1 + (i % 2), ::GetColor(brightness, brightness, brightness + 15), TRUE);
	}

	// SFのHUD感を演出するための四隅ブラケット
	unsigned int hudColor = ::GetColor(0, 160, 255);
	int bracketLen = 20;
	DrawLine(5, 5, 5 + bracketLen, 5, hudColor);                                                                      // 左上横
	DrawLine(5, 5, 5, 5 + bracketLen, hudColor);                                                                      // 左上縦
	DrawLine(SCREEN_WIDTH - 5, 5, SCREEN_WIDTH - 5 - bracketLen, 5, hudColor);                                        // 右上横
	DrawLine(SCREEN_WIDTH - 5, 5, SCREEN_WIDTH - 5, 5 + bracketLen, hudColor);                                        // 右上縦
	DrawLine(5, SCREEN_HEIGHT - 5, 5 + bracketLen, SCREEN_HEIGHT - 5, hudColor);                                      // 左下横
	DrawLine(5, SCREEN_HEIGHT - 5, 5, SCREEN_HEIGHT - 5 - bracketLen, hudColor);                                      // 左下縦
	DrawLine(SCREEN_WIDTH - 5, SCREEN_HEIGHT - 5, SCREEN_WIDTH - 5 - bracketLen, SCREEN_HEIGHT - 5, hudColor);        // 右下横
	DrawLine(SCREEN_WIDTH - 5, SCREEN_HEIGHT - 5, SCREEN_WIDTH - 5, SCREEN_HEIGHT - 5 - bracketLen, hudColor);        // 右下縦

	// 左側パネル（サイバーセキュリティ・モニター）
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 50);
		DrawBox(10, ROOM_TOP, ROOM_LEFT - 20, ROOM_BOTTOM, ::GetColor(15, 25, 40), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		
		DrawBox(10, ROOM_TOP, ROOM_LEFT - 20, ROOM_BOTTOM, ::GetColor(0, 100, 180), FALSE);
		DrawBox(12, ROOM_TOP + 2, ROOM_LEFT - 22, ROOM_BOTTOM - 2, ::GetColor(0, 60, 120), FALSE);

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
		DrawString(20, textY + 32, L"DATABASE: LATEST", ::GetColor(0, 220, 255));
		DrawString(20, textY + 48, L"LEVEL: SAFE", ::GetColor(0, 255, 130));

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
	}

	// 右側パネル（神秘のオカルトエンジン）
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
		DrawString(ROOM_RIGHT + 30, rightTextY + 16, L"INTEGRITY: 100%", ::GetColor(180, 40, 255));
		DrawString(ROOM_RIGHT + 30, rightTextY + 32, L"STATUS: STABLE", ::GetColor(0, 255, 130));
	}

	// リーダーボードの半透明ガラスモーフィズム中央パネル
	{
		// 背景半透明
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		DrawBox(240, 150, 1040, 620, ::GetColor(18, 28, 44), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// ネオングロー境界線
		unsigned int borderColor = ::GetColor(0, 160, 255); // サイバーネオン水色
		for (int i = 0; i < 4; ++i)
		{
			int alpha = 255 - (i * 50);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			DrawBox(240 - 4 + i, 150 - 4 + i, 1040 + 4 - i, 620 + 4 - i, borderColor, FALSE);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// デジタル方眼グリッド
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 30);
		unsigned int gridColor = ::GetColor(80, 110, 140);
		for (int x = 240 + 40; x < 1040; x += 60)
		{
			DrawLine(x, 150, x, 620, gridColor);
		}
		for (int y = 150 + 40; y < 620; y += 60)
		{
			DrawLine(240, y, 1040, y, gridColor);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// 7. タイトルテキスト表示
	SetFontSize(44);
	const GameDifficulty viewDiff = RankingManager::GetViewDifficulty();
	const wchar_t* diffStr = STR_DIFF_NORMAL;
	if (viewDiff == GameDifficulty::Easy) diffStr = STR_DIFF_EASY;
	else if (viewDiff == GameDifficulty::Hell) diffStr = STR_DIFF_HELL;

	wchar_t fullTitle[64];
	swprintf_s(fullTitle, L"%s %s", STR_RANKING_TITLE, diffStr);
	const int titleWidth = GetDrawStringWidth(fullTitle, -1);
	DrawFormatString(SCREEN_WIDTH / 2 - titleWidth / 2, 30, ::GetColor(255, 220, 110), L"%s", fullTitle);

	// 8. 難易度タブ切り替えボタン
	SetFontSize(20);
	UiMouse::DrawButton(BTN_TAB_EASY_LEFT, BTN_TAB_TOP, BTN_TAB_EASY_RIGHT, BTN_TAB_BOTTOM, STR_DIFF_EASY, true, viewDiff == GameDifficulty::Easy);
	UiMouse::DrawButton(BTN_TAB_NORMAL_LEFT, BTN_TAB_TOP, BTN_TAB_NORMAL_RIGHT, BTN_TAB_BOTTOM, STR_DIFF_NORMAL, true, viewDiff == GameDifficulty::Normal);
	UiMouse::DrawButton(BTN_TAB_HELL_LEFT, BTN_TAB_TOP, BTN_TAB_HELL_RIGHT, BTN_TAB_BOTTOM, STR_DIFF_HELL, true, viewDiff == GameDifficulty::Hell);

	// 9. ランキング表ヘッダー
	SetFontSize(22);
	DrawFormatString(280, 165, ::GetColor(170, 210, 255), STR_RANKING_COL_RANK);
	DrawFormatString(360, 165, ::GetColor(170, 210, 255), STR_RANKING_COL_NAME);
	DrawFormatString(560, 165, ::GetColor(170, 210, 255), STR_RANKING_COL_SCORE);
	DrawFormatString(700, 165, ::GetColor(170, 210, 255), STR_RANKING_COL_COMBO);

	// ヘッダー下仕切り線
	DrawLine(260, 195, 1020, 195, ::GetColor(0, 120, 200));

	const int entryCount = RankingManager::GetEntryCount();
	const int highlightIndex = RankingManager::GetHighlightIndex();

	if (entryCount <= 0)
	{
		SetFontSize(26);
		DrawFormatString(
			SCREEN_WIDTH / 2 - 200,
			300,
			::GetColor(140, 160, 180),
			STR_RANKING_EMPTY);
	}
	else
	{
		for (int i = 0; i < entryCount; ++i)
		{
			const RankingEntry& entry = RankingManager::GetEntry(i);
			const int rowY = 215 + i * 38;
			const bool isHighlighted = (i == highlightIndex);
		// ランクインエントリをゴールドでハイライトして識別しやすくする
			const int textColor = isHighlighted
				? ::GetColor(255, 220, 80)
				: ::GetColor(230, 240, 255);

			DrawFormatString(280, rowY, textColor, L"%2d.", i + 1);
			DrawFormatString(360, rowY, textColor, L"%s", entry.name);
			DrawFormatString(560, rowY, textColor, L"%7d", entry.score);
			DrawFormatString(700, rowY, textColor, L"x%d", entry.maxCombo);
		}
	}

	if (highlightIndex >= 0)
	{
		SetFontSize(20);
		DrawFormatString(
			SCREEN_WIDTH / 2 - 180,
			590,
			::GetColor(255, 220, 80),
			STR_RANKING_NEW_ENTRY);
	}

	// 10. フッターボタンとデータクリアボタン
	UiMouse::DrawButton(BTN_LEFT, BTN_BACK_TOP, BTN_RIGHT, BTN_BACK_BOTTOM, STR_BTN_BACK);
	
	SetFontSize(20);
	UiMouse::DrawButton(BTN_CLEAR_LEFT, BTN_CLEAR_TOP, BTN_CLEAR_RIGHT, BTN_CLEAR_BOTTOM, STR_BTN_CLEAR_DATA);

	// カスタムカーソルを最前面に描画
	UiMouse::DrawCursor(GameSession::GetCursorRadius(), false);
}

SceneID RankingScene::GetNextSceneID() const
{
	if (m_requestReturn)
	{
		return RankingManager::GetReturnScene();
	}
	return SceneID::None;
}
