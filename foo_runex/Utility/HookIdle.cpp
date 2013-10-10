#include "stdafx.h"
#include "hookidle.h"
#include <list>
using namespace Utils;

namespace Utils{ namespace {

	typedef std::list<CHookIdle*> TLstHookIdle;

	struct XHookIdleData
	{
		LPVOID _prevToken;
		TLstHookIdle::iterator _iHI;
		TLstHookIdle::iterator _endHI;
		int _code; WPARAM _wParam; LPARAM _lParam;
	};
	
	TLstHookIdle g_lsthooks;

	HHOOK g_hHook = NULL;

	
	//*** The windows hook procedure ***//
	LRESULT CALLBACK ForegroundIdleProc(int code, WPARAM wParam, LPARAM lParam)
	{
		if(code != HC_ACTION)
			return CallNextHookEx(g_hHook, code, wParam, lParam);
		
		XHookIdleData xd;
		xd._prevToken = NULL;
		xd._iHI = g_lsthooks.begin();
		xd._endHI = g_lsthooks.end();
		xd._code = code; xd._wParam = wParam; xd._lParam = lParam;

		return CHookIdle::Call(&xd);
	}


}}

using namespace Utils;

CHookIdle::CHookIdle()
{
	if(g_lsthooks.size() == 0)
		g_hHook = SetWindowsHookEx(WH_FOREGROUNDIDLE,ForegroundIdleProc, NULL, AfxGetThread()->m_nThreadID);
	g_lsthooks.push_front(this);
	_token = NULL;
}

CHookIdle::~CHookIdle(void)
{
	Clear();
}

bool CHookIdle::Clear()
{
	g_lsthooks.remove(this);
	if(g_lsthooks.size() == 0)
	{
		UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
	}
	return true;
}

LRESULT CHookIdle::Call(LPVOID token)
{
	XHookIdleData& xd = *(XHookIdleData*)token;
	if(xd._iHI == xd._endHI)
		return CallNextHookEx(g_hHook, xd._code, xd._wParam, xd._lParam);
	CHookIdle* pHI = *xd._iHI;
	PHookIdle pKeep(pHI);
	xd._iHI++;
	xd._prevToken = pHI->_token;
	pHI->_token = token;
	LRESULT result = pHI->OnIdle();
	pHI->_token = xd._prevToken;
	return result;
}

LRESULT CHookIdle::OnIdle()
{
	return Default();
}

LRESULT CHookIdle::Default()
{
	return Call(_token);	
}
