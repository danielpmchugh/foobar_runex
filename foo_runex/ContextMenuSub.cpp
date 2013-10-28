#include "stdafx.h"

#include "Utility\wndsubclasser.h"
#include "Utility\WndProcHook.h"
#include "Utility\SmartPtr.h"
#include <list>
#include <map>
#include <afxmt.h>
#include "Utility\STrace.h"
#include "ContextMenuSub.h"

using namespace Utils;
using namespace Safe;
using namespace Mfc;



CContextMenuSub::CContextMenuSub(int iWMMenuItemClick, int iBtnPos, int iWMShowHideToolbar, HWND hWndNotify)
{
	m_iWMMenuItemClick = iWMMenuItemClick;
	m_iBtnPos = iBtnPos ;
	m_iWMShowHideToolbar = iWMShowHideToolbar;
	m_hWndNotify = hWndNotify ;
}


CContextMenuSub::CContextMenuSub(void)
{
}


CContextMenuSub::~CContextMenuSub(void)
{
}

LRESULT CContextMenuSub::WndProcSub(UINT uMsg, WPARAM wParam, LPARAM lParam)
{	

	if ((uMsg == m_iWMMenuItemClick) && ((t_int32)LOWORD(wParam) == m_iBtnPos))
	{
//#ifdef _DEBUG
		console::formatter() << "WM=" << (t_int32)uMsg  << " WPARAM=" << (t_int32)HIWORD(wParam) << "-" << (t_int32)LOWORD(wParam) << " LPARAM=" << (t_int32)HIWORD(lParam) << "-" << LOWORD(lParam ) ;	  			
//#endif
		// send toolbar message when context menu item is click- signaling user want to hide/show toolbar button
		SendMessage(m_hWndNotify, m_iWMShowHideToolbar, 0, 0);
	}
	  		  
	return Default();
}