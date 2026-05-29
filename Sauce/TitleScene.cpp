#include "TitleScene.h"

#include "GameConfig.h"
#include "GameStrings.h"
#include "GameSession.h"
#include "RankingManager.h"
#include "UiMouse.h"
#include "DxLib.h"
#include "LoadingScene.h"
#include <cmath>

namespace
{
	static const int BTN_LEFT = 440;
	static const int BTN_RIGHT = 840;
	static const int BTN_PLAY_TOP = 410;
	static const int BTN_PLAY_BOTTOM = 460;
	static const int BTN_SETTING_TOP = 475;
	static const int BTN_SETTING_BOTTOM = 525;
	static const int BTN_RANK_TOP = 540;
	static const int BTN_RANK_BOTTOM = 590;
	static const int BTN_QUIT_TOP = 605;
	static const int BTN_QUIT_BOTTOM = 655;

	// 難易度選択ボタン座標（3種類+キャンセルが収まるよう間隔を調整済み）
	static const int BTN_PLAY_EASY_TOP = 420;
	static const int BTN_PLAY_EASY_BOTTOM = 465;
	static const int BTN_PLAY_NORMAL_TOP = 480;
	static const int BTN_PLAY_NORMAL_BOTTOM = 525;
	static const int BTN_PLAY_HELL_TOP = 540;
	static const int BTN_PLAY_HELL_BOTTOM = 585;
	static const int BTN_CANCEL_TOP = 600;
	static const int BTN_CANCEL_BOTTOM = 645;

	// タイトル画面の背景装飾に使用するゲームオブジェクト描画ヘルパー
	void DrawHackDecor(float x, float y, float radius, float angle)
	{
		unsigned int colorCyan1   = ::GetColor(0, 245, 255);
		unsigned int colorCyan2   = ::GetColor(0, 120, 200);
		unsigned int colorCyan3   = ::GetColor(220, 255, 255);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
		DrawCircle((int)(x + 5), (int)(y + 5), (int)radius, ::GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawCircle((int)x, (int)y, (int)radius, colorCyan2, FALSE);
		DrawCircle((int)x, (int)y, (int)(radius * 0.75f), colorCyan1, FALSE);
		DrawCircle((int)x, (int)y, (int)(radius * 0.35f), colorCyan1, TRUE);
		DrawCircle((int)x, (int)y, (int)(radius * 0.15f), colorCyan3, TRUE);

		float tickR = radius * 1.0f;
		for (int k = 0; k < 4; ++k)
		{
			float tAngle = angle + k * (3.14159265f / 2.0f);
			int px = (int)(x + std::cos(tAngle) * tickR);
			int py = (int)(y + std::sin(tAngle) * tickR);
			DrawLine(px - 5, py, px + 5, py, colorCyan1);
			DrawLine(px, py - 5, px, py + 5, colorCyan1);
		}

		DrawLine((int)(x - radius * 1.1f), (int)y, (int)(x - radius * 0.5f), (int)y, colorCyan1);
		DrawLine((int)(x + radius * 0.5f), (int)y, (int)(x + radius * 1.1f), (int)y, colorCyan1);
		DrawLine((int)x, (int)(y - radius * 1.1f), (int)x, (int)(y - radius * 0.5f), colorCyan1);
		DrawLine((int)x, (int)(y + radius * 0.5f), (int)x, (int)(y + radius * 1.1f), colorCyan1);
	}

	void DrawBombDecor(float x, float y, float radius, float angle)
	{
		unsigned int colorOrange1 = ::GetColor(255, 90, 0);
		unsigned int colorOrange2 = ::GetColor(255, 190, 0);
		unsigned int colorOrange3 = ::GetColor(255, 255, 100);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
		DrawCircle((int)(x + 5), (int)(y + 5), (int)radius, ::GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawCircle((int)x, (int)y, (int)radius, colorOrange1, FALSE);

		int numTeeth = 8;
		for (int k = 0; k < numTeeth; ++k)
		{
			float tAngle = angle + k * (2.0f * 3.14159265f / numTeeth);
			int x1 = (int)(x + std::cos(tAngle - 0.15f) * (radius * 0.7f));
			int y1 = (int)(y + std::sin(tAngle - 0.15f) * (radius * 0.7f));
			int x2 = (int)(x + std::cos(tAngle + 0.15f) * (radius * 0.7f));
			int y2 = (int)(y + std::sin(tAngle + 0.15f) * (radius * 0.7f));
			int x3 = (int)(x + std::cos(tAngle) * (radius * 1.05f));
			int y3 = (int)(y + std::sin(tAngle) * (radius * 1.05f));
			DrawTriangle(x1, y1, x2, y2, x3, y3, colorOrange2, TRUE);
		}

		DrawCircle((int)x, (int)y, (int)(radius * 0.75f), colorOrange1, TRUE);
		DrawCircle((int)x, (int)y, (int)(radius * 0.45f), colorOrange2, TRUE);
		DrawCircle((int)x, (int)y, (int)(radius * 0.2f), colorOrange3, TRUE);
	}

	void DrawRitualDecor(float x, float y, float radius, float angle)
	{
		unsigned int colorPurple1 = ::GetColor(170, 50, 255);
		unsigned int colorPurple2 = ::GetColor(255, 0, 150);
		unsigned int colorPurple3 = ::GetColor(240, 220, 255);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
		DrawCircle((int)(x + 5), (int)(y + 5), (int)radius, ::GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawCircle((int)x, (int)y, (int)radius, colorPurple1, FALSE);
		DrawCircle((int)x, (int)y, (int)(radius * 0.94f), colorPurple1, FALSE);
		DrawCircle((int)x, (int)y, (int)(radius * 0.65f), colorPurple2, FALSE);

		int px[5], py[5];
		for (int k = 0; k < 5; ++k)
		{
			float tAngle = angle + k * (4.0f * 3.14159265f / 5.0f);
			px[k] = (int)(x + std::cos(tAngle) * (radius * 0.88f));
			py[k] = (int)(y + std::sin(tAngle) * (radius * 0.88f));
		}
		for (int k = 0; k < 5; ++k)
		{
			DrawLine(px[k], py[k], px[(k + 1) % 5], py[(k + 1) % 5], colorPurple2);
		}

		int numTri = 3;
		for (int k = 0; k < numTri; ++k)
		{
			float tAngle = -angle + k * (2.0f * 3.14159265f / numTri);
			int x1 = (int)(x + std::cos(tAngle) * (radius * 0.55f));
			int y1 = (int)(y + std::sin(tAngle) * (radius * 0.55f));
			int x2 = (int)(x + std::cos(tAngle + 2.0f * 3.14159265f / 3.0f) * (radius * 0.55f));
			int y2 = (int)(y + std::sin(tAngle + 2.0f * 3.14159265f / 3.0f) * (radius * 0.55f));
			DrawLine(x1, y1, x2, y2, colorPurple1);
		}

		DrawCircle((int)x, (int)y, (int)(radius * 0.25f), colorPurple3, TRUE);
	}

	void DrawSnackDecor(float x, float y, float radius, float angle)
	{
		unsigned int colorPink1   = ::GetColor(255, 130, 170);
		unsigned int colorPink2   = ::GetColor(255, 215, 80);
		unsigned int colorPink3   = ::GetColor(255, 255, 240);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
		DrawCircle((int)(x + 5), (int)(y + 5), (int)radius, ::GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		float cosA = std::cos(angle);
		float sinA = std::sin(angle);

		auto drawRotatedTriangle = [&](float lx1, float ly1, float lx2, float ly2, float lx3, float ly3, unsigned int col) {
			int x1 = (int)(x + lx1 * cosA - ly1 * sinA);
			int y1 = (int)(y + lx1 * sinA + ly1 * cosA);
			int x2 = (int)(x + lx2 * cosA - ly2 * sinA);
			int y2 = (int)(y + lx2 * sinA + ly2 * cosA);
			int x3 = (int)(x + lx3 * cosA - ly3 * sinA);
			int y3 = (int)(y + lx3 * sinA + ly3 * cosA);
			DrawTriangle(x1, y1, x2, y2, x3, y3, col, TRUE);
		};

		drawRotatedTriangle(-radius * 0.4f, 0.0f, -radius * 0.95f, -radius * 0.45f, -radius * 0.95f, radius * 0.45f, colorPink2);
		drawRotatedTriangle(-radius * 0.4f, 0.0f, -radius * 0.85f, -radius * 0.25f, -radius * 0.85f, radius * 0.25f, colorPink3);

		drawRotatedTriangle(radius * 0.4f, 0.0f, radius * 0.95f, -radius * 0.45f, radius * 0.95f, radius * 0.45f, colorPink2);
		drawRotatedTriangle(radius * 0.4f, 0.0f, radius * 0.85f, -radius * 0.25f, radius * 0.85f, radius * 0.25f, colorPink3);

		DrawCircle((int)x, (int)y, (int)(radius * 0.62f), colorPink1, TRUE);

		int numSwirls = 4;
		for (int k = 0; k < numSwirls; ++k)
		{
			float swirlAngle = angle + k * (2.0f * 3.14159265f / numSwirls);
			int xStart = (int)x;
			int yStart = (int)y;
			int xEnd = (int)(x + std::cos(swirlAngle) * (radius * 0.55f));
			int yEnd = (int)(y + std::sin(swirlAngle) * (radius * 0.55f));
			DrawLine(xStart, yStart, xEnd, yEnd, colorPink3);
		}

		DrawCircle((int)(x - radius * 0.2f), (int)(y - radius * 0.2f), (int)(radius * 0.15f), colorPink3, TRUE);
	}
}

void TitleScene::Init()
{
	m_mode = TitleMode::Normal;
	m_requestGoMain = false;
	m_requestGoRanking = false;
	m_requestGoSetting = false;
	m_nameInput.Reset();
}

void TitleScene::Update()
{
	UiMouse::UpdateFrame();

	if (m_mode == TitleMode::SelectingDifficulty)
	{
		if (UiMouse::TryClick(BTN_LEFT, BTN_PLAY_EASY_TOP, BTN_RIGHT, BTN_PLAY_EASY_BOTTOM))
		{
			GameSession::SetDifficulty(GameDifficulty::Easy);
			m_requestGoMain = true;
		}
		if (UiMouse::TryClick(BTN_LEFT, BTN_PLAY_NORMAL_TOP, BTN_RIGHT, BTN_PLAY_NORMAL_BOTTOM))
		{
			GameSession::SetDifficulty(GameDifficulty::Normal);
			m_requestGoMain = true;
		}
		if (UiMouse::TryClick(BTN_LEFT, BTN_PLAY_HELL_TOP, BTN_RIGHT, BTN_PLAY_HELL_BOTTOM))
		{
			GameSession::SetDifficulty(GameDifficulty::Hell);
			m_requestGoMain = true;
		}
		if (UiMouse::TryClick(BTN_LEFT, BTN_CANCEL_TOP, BTN_RIGHT, BTN_CANCEL_BOTTOM))
		{
			m_mode = TitleMode::Normal;
		}
		return;
	}
	else
	{
		if (UiMouse::TryClick(BTN_LEFT, BTN_PLAY_TOP, BTN_RIGHT, BTN_PLAY_BOTTOM))
		{
			m_mode = TitleMode::SelectingDifficulty;
		}
		if (UiMouse::TryClick(BTN_LEFT, BTN_SETTING_TOP, BTN_RIGHT, BTN_SETTING_BOTTOM))
		{
			m_requestGoSetting = true;
		}
		if (UiMouse::TryClick(BTN_LEFT, BTN_RANK_TOP, BTN_RIGHT, BTN_RANK_BOTTOM))
		{
			RankingManager::ClearHighlight();
			RankingManager::SetReturnScene(SceneID::Title);
			m_requestGoRanking = true;
		}
		if (UiMouse::TryClick(BTN_LEFT, BTN_QUIT_TOP, BTN_RIGHT, BTN_QUIT_BOTTOM))
		{
			GameSession::RequestQuit();
		}
	}
}

void TitleScene::Draw()
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
		DrawString(20, textY + 32, L"THREATS: --", ::GetColor(140, 170, 210));
		DrawString(20, textY + 48, L"LEVEL: STANDBY", ::GetColor(0, 255, 255));

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
		DrawString(ROOM_RIGHT + 30, rightTextY + 16, L"INTEGRITY: 100%", ::GetColor(180, 40, 255));
		DrawString(ROOM_RIGHT + 30, rightTextY + 32, L"STATUS: READY", ::GetColor(0, 255, 130));
	}

	// プレイ中に出現するゲームオブジェクトをタイトル画面で浮遊させて演出する
	{
		float time = (float)GetNowCount();
		DrawHackDecor(260.0f,  300.0f + std::sin(time * 0.0022f) * 12.0f, 35.0f, time *  0.0016f);
		DrawBombDecor(480.0f,  290.0f + std::cos(time * 0.0024f) * 10.0f, 30.0f, time *  0.0020f);
		DrawRitualDecor(800.0f, 295.0f + std::sin(time * 0.0020f + 1.0f) * 11.0f, 33.0f, time * -0.0014f);
		DrawSnackDecor(1020.0f, 305.0f + std::cos(time * 0.0022f + 1.2f) * 12.0f, 28.0f, time *  0.0018f);
	}

	// 中央メニューのガラスモーフィズムパネル
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		DrawBox(200, 380, SCREEN_WIDTH - 200, SCREEN_HEIGHT - 40, ::GetColor(18, 28, 44), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// 難易度選択中はボーダーをピンクに変えてモード変化を視覚的に伝える
		unsigned int borderColor = ::GetColor(0, 160, 255);
		if (m_mode == TitleMode::SelectingDifficulty)
		{
			borderColor = ::GetColor(255, 50, 180);
		}
		// アルファをずらしてグロー効果を出す
		for (int i = 0; i < 4; ++i)
		{
			int alpha = 255 - (i * 50);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			DrawBox(200 - 4 + i, 380 - 4 + i, SCREEN_WIDTH - 200 + 4 - i, SCREEN_HEIGHT - 40 + 4 - i, borderColor, FALSE);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 30);
		unsigned int gridColor = ::GetColor(80, 110, 140);
		for (int x = 200 + 30; x < SCREEN_WIDTH - 200; x += 60)
		{
			DrawLine(x, 380, x, SCREEN_HEIGHT - 40, gridColor);
		}
		for (int y = 380 + 30; y < SCREEN_HEIGHT - 40; y += 60)
		{
			DrawLine(200, y, SCREEN_WIDTH - 200, y, gridColor);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	SetFontSize(52);
	const int titleWidth = GetDrawStringWidth(STR_TITLE_MAIN, -1);
	DrawFormatString(SCREEN_WIDTH / 2 - titleWidth / 2, 120, ::GetColor(255, 220, 110), STR_TITLE_MAIN);

	SetFontSize(26);
	const int subWidth = GetDrawStringWidth(STR_TITLE_SUB, -1);
	DrawFormatString(SCREEN_WIDTH / 2 - subWidth / 2, 200, ::GetColor(170, 210, 255), STR_TITLE_SUB);

	// モードに応じてボタンセットを切り替えて描画する
	if (m_mode == TitleMode::SelectingDifficulty)
	{
		UiMouse::DrawButton(BTN_LEFT, BTN_PLAY_EASY_TOP, BTN_RIGHT, BTN_PLAY_EASY_BOTTOM, STR_BTN_PLAY_EASY);
		UiMouse::DrawButton(BTN_LEFT, BTN_PLAY_NORMAL_TOP, BTN_RIGHT, BTN_PLAY_NORMAL_BOTTOM, STR_BTN_PLAY_NORMAL);
		UiMouse::DrawButton(BTN_LEFT, BTN_PLAY_HELL_TOP, BTN_RIGHT, BTN_PLAY_HELL_BOTTOM, STR_BTN_PLAY_HELL);
		UiMouse::DrawButton(BTN_LEFT, BTN_CANCEL_TOP, BTN_RIGHT, BTN_CANCEL_BOTTOM, STR_BTN_CANCEL);
	}
	else
	{
		UiMouse::DrawButton(BTN_LEFT, BTN_PLAY_TOP, BTN_RIGHT, BTN_PLAY_BOTTOM, STR_BTN_PLAY);
		UiMouse::DrawButton(BTN_LEFT, BTN_SETTING_TOP, BTN_RIGHT, BTN_SETTING_BOTTOM, L"\u8A2D\u5B9A");
		UiMouse::DrawButton(BTN_LEFT, BTN_RANK_TOP, BTN_RIGHT, BTN_RANK_BOTTOM, STR_BTN_RANKING);
		UiMouse::DrawButton(BTN_LEFT, BTN_QUIT_TOP, BTN_RIGHT, BTN_QUIT_BOTTOM, STR_BTN_QUIT);
	}

	// カスタムカーソルを最前面に描画（当たり判定円はタイトルでは非表示）
	UiMouse::DrawCursor(GameSession::GetCursorRadius(), false);
}

SceneID TitleScene::GetNextSceneID() const
{
	if (m_requestGoRanking) return SceneID::Ranking;
	if (m_requestGoMain)
	{
		LoadingScene::SetTargetScene(SceneID::Main);
		return SceneID::Loading;
	}
	if (m_requestGoSetting) return SceneID::Setting;
	return SceneID::None;
}