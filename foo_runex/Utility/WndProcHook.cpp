#include "stdafx.h"
#include "WndProcHook.h"
#include <list>
#include <afxmt.h>
#include "STrace.h"
#include "RunExWnd.h"
using namespace Utils;

namespace Utils{namespace {
	
	typedef std::list<CWndProcHook*> TWndProcHookChain;
	TWndProcHookChain g_wndProcHookChain;

	HHOOK g_wndProcHookChainHook=0;

	struct OnHookToken
	{
		TWndProcHookChain::iterator i;
		int nCode;
		WPARAM wParam;
		LPARAM lParam;
		LPVOID prevtoken;
	};

	LRESULT CALLBACK CallHookWndProc(
	int nCode,      // hook code
	WPARAM wParam,  // current-process flag
	LPARAM lParam   // message data
	)
	{
		if(nCode < 0) 
			return CallNextHookEx(g_wndProcHookChainHook, nCode, wParam, lParam);
		
		static CCriticalSection ccs;
		CSingleLock lock(&ccs, true);

		//one thread at time please !
		OnHookToken oht;
		oht.i = g_wndProcHookChain.begin();
		oht.nCode = nCode;
		oht.wParam = wParam;
		oht.lParam = lParam;
		oht.prevtoken = NULL;

		return CWndProcHook::Call(&oht);
 	}
}}

CWndProcHook::CWndProcHook()
{

}

int CWndProcHook::CreateHook()
{
#ifdef _DEBUG
	Mfc::STrace trc; trc.Trace(typeid(*this), this, "CWndProcHook(void)");
#endif
	_token = NULL;
	if (!g_wndProcHookChain.size())
		g_wndProcHookChainHook = SetWindowsHookEx(WH_CALLWNDPROC, CallHookWndProc, NULL, GetCurrentThreadId());
	g_wndProcHookChain.push_front(this);
	return 0;
}

CWndProcHook::~CWndProcHook(void)
{
#ifdef _DEBUG
	Mfc::STrace trc; trc.Trace(typeid(*this), this, "~CWndProcHook(void)");
#endif
	Clear();
}

bool CWndProcHook::Clear()
{
	if (!g_wndProcHookChain.empty())
	{
		g_wndProcHookChain.remove(this);
		if(!g_wndProcHookChain.size())
			UnhookWindowsHookEx(g_wndProcHookChainHook);
	}

	return true;
}


LRESULT CWndProcHook::Call(LPVOID token)
{
	OnHookToken* pOht = (OnHookToken*)token;
	if(pOht->i == g_wndProcHookChain.end())
		return CallNextHookEx(g_wndProcHookChainHook, pOht->nCode, pOht->wParam, pOht->lParam);

	CWndProcHook* pWH = *pOht->i;
//	PWndProcHook pKeep(pWH);
	pOht->i++;
	pOht->prevtoken = pWH->_token;
	pWH->_token = token;
	LRESULT r = pWH->OnHook(*(CWPSTRUCT*)pOht->lParam);
	pWH->_token = pOht->prevtoken;
	return r;
}


LRESULT CWndProcHook::Default()
{
	return Call(_token);
}

