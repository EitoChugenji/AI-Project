#include "DxLib.h"

#include "GameConfig.h"
#include "GameStrings.h"
#include "GameSession.h"
#include "SceneManager.h"

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	ChangeWindowMode(TRUE);
	SetMainWindowText(STR_WINDOW_TITLE);
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);

	if (DxLib_Init() == -1)
	{
		return -1;
	}

	SetWaitVSyncFlag(TRUE);

	SceneManager sceneManager;
	sceneManager.SetFirstScene(SceneID::Title);

	while (ProcessMessage() == 0)
	{
		sceneManager.Update();
		if (GameSession::ConsumeQuitRequest())
		{
			break;
		}
		sceneManager.Draw();
		ScreenFlip();
	}

	DxLib_End();
	return 0;
}
