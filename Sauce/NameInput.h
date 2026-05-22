#pragma once

#include "GameConfig.h"

class NameInput
{
public:
	void Reset();
	void Update(int panelX, int panelY);
	void Draw(int x, int y, int labelColor, int textColor) const;
	const wchar_t* GetText() const;
	bool HasText() const;

private:
	void AppendChar(wchar_t ch);
	bool TryClickKey(int left, int top, int right, int bottom);

	wchar_t m_buffer[RANKING_NAME_MAX_LEN + 1];
	int m_length;
};
