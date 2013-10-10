#include "stdafx.h"

#include "wndsubclasser.h"
#include "WndProcHook.h"
#include "SmartPtr.h"
#include <list>
#include <map>
#include <afxmt.h>
#include "STrace.h"
using namespace Utils;
using namespace Safe;
using namespace Mfc;

namespace Utils{namespace{
	
	typedef std::list<CWndSubclasser*> TLstpWndSubClasser;
	typedef std::map<HWND, TLstpWndSubClasser> TMapHwndLstSubClasser;
	typedef std::map<HWND, WNDPROC> TMapHwndOldProc;

	TMapHwndLstSubClasser g_map;
	TMapHwndOldProc g_prcmap;

	struct SWndSubclasserParams
	{
		TLstpWndSubClasser::iterator i;
		TLstpWndSubClasser::iterator end;
		HWND hWnd; UINT uMsg; WPARAM wParam; LPARAM lParam;
		LPVOID prevtoken;
	};

	LRESULT CALLBACK SubWindowProc(
	HWND hwnd,      // handle to window
	UINT uMsg,      // message identifier
	WPARAM wParam,  // first message parameter
	LPARAM lParam   // second message parameter
	)
	{
		TMapHwndLstSubClasser::iterator mapI = g_map.find(hwnd);
		ASSERT(mapI != g_map.end());

		static CCriticalSection ccs;
		CSingleLock lock(&ccs, true);

        SWndSubclasserParams prms;
		TLstpWndSubClasser& lst = mapI->second;
		prms.i = lst.begin();
		prms.end = lst.end();
		prms.hWnd = hwnd; prms.uMsg = uMsg; prms.wParam = wParam; prms.lParam = lParam;
		prms.prevtoken = NULL;
			
		
		return CWndSubclasser::Call(&prms);
	}

}}

CWndSubclasser::CWndSubclasser()
{
#ifdef _DEBUG
	Mfc::STrace trc; trc.Trace(typeid(*this),this,"CWndSubclasser");
#endif
	_hWnd = NULL;
	_token = NULL;
	_bCleared = false;
}

CWndSubclasser::~CWndSubclasser(void)
{
#ifdef _DEBUG
	Mfc::STrace trc; trc.Trace(typeid(*this),this,"~CWndSubclasser");
#endif
	
}

#pragma warning(push)
#pragma warning(disable: 4244 4312)
bool CWndSubclasser::Subclass(HWND hWnd)
{
	if(!hWnd || _hWnd) return false;
	_hWnd = hWnd;
	_bCleared = false;
	TLstpWndSubClasser& lst = g_map[hWnd];
	lst.push_front(this);

	WNDPROC& oldPrc = g_prcmap[hWnd];
	if(!oldPrc) //not yet subclassed
	{
		oldPrc = (WNDPROC) GetWindowLongPtr(hWnd, GWL_WNDPROC);
		SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)SubWindowProc);
	}
	
	return true;
}

bool CWndSubclasser::Clear()
{
	if(!_hWnd) return false;
	_bCleared = true;

	TLstpWndSubClasser& lst = g_map[_hWnd];
	lst.remove(this);
	
	if(lst.size() == 0)
	{
		g_map.erase(_hWnd);
		SetWindowLongPtr(_hWnd, GWL_WNDPROC, (LONG_PTR)g_prcmap[_hWnd]);
		g_prcmap.erase(_hWnd);
	}

	return true;
}
#pragma warning(pop)


LRESULT CWndSubclasser::Call(LPVOID token)
{
	SWndSubclasserParams* pPrms = (SWndSubclasserParams*)token;
	
	if(pPrms->i == pPrms->end) 
		//finished the list: just call the original window procedure. 
		//This will go into AfxWndProc and hence in CWnd::OnWndMsg and into the message map.
		return CallWindowProc(g_prcmap[pPrms->hWnd], pPrms->hWnd, pPrms->uMsg, pPrms->wParam, pPrms->lParam);
	
	CWndSubclasser* pWS = *pPrms->i;
	PWndSubclasser pKeep(pWS);
	pPrms->i++; // the nextime Call is called, will process the next in list
    pPrms->prevtoken = pWS->_token; //save for later
	pWS->_token = token; //remember for Defauklt calling
	LRESULT r=0;
	if(pPrms->uMsg == WM_NCDESTROY)
		pWS->_bCleared = true; //will not destroy yet
	
	if(pWS->_bCleared) r = pWS->Default(); //skip a declared as "destroyed"
	else r = pWS->WndProcSub(pPrms->uMsg, pPrms->wParam, pPrms->lParam); //just call the subproc
	pWS->_token = pPrms->prevtoken; //restore previous token (naw can return also from recursion)
	return r;
}

LRESULT CWndSubclasser::WndProcSub(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//normally overridden.
	// by default ... does Default :-))

	  		  
	return Default();
}

LRESULT CWndSubclasser::Default()
{
	LRESULT r = Call(_token); //will process the next il list (if any)
	return r;
}

LRESULT CWndSubclasser::Default(WPARAM wParam, LPARAM lParam)
{
	SWndSubclasserParams* pPrms = (SWndSubclasserParams*)_token;
	pPrms->wParam = wParam; pPrms->lParam = lParam; //modify parameters
	LRESULT r = Call(_token); //will process the next il list (if any)
	return r;
}


