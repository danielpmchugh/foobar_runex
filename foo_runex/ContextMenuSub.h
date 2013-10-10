#pragma once
#include "Utility\WndSubclasser.h"

class CContextMenuSub :
	public Utils::CWndSubclasser
{
public:
	CContextMenuSub(int, int, int ,HWND);
	~CContextMenuSub(void);

private:
	virtual LRESULT WndProcSub(UINT uMsg, WPARAM wParam, LPARAM lParam);

	CContextMenuSub(void);

	int m_iWMMenuItemClick;
	int m_iBtnPos;
	int m_iWMShowHideToolbar;
	HWND m_hWndNotify;

};

