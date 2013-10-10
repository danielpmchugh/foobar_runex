#ifndef RUNEXWND_H
#define RUNEXWND_H
#include "stdafx.h"
#include "window_helper.h"
#include "Utility/WndSubclasser.h"2
#include "Utility/WndProcHook.h"2




class CRunExWnd :
	public CSimpleWindowImpl<CRunExWnd>,
	private message_filter
{
public:
	static CWnd * contextWnd;
	static HWND hToolbar;
	static HWND hCmpWnd;
	static CToolBar m_FirstToolBar;
	typedef CSimpleWindowImpl<CRunExWnd> super;
	
	static void ShowWindow();
	static void HideWindow();

	// Dispatches window messages to the appropriate handler functions.
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & lResult);

	// Message handler functions.
	// The function signatures are intended to be compatible with the MSG_WM_* macros in WTL.
	LRESULT OnCreate(LPCREATESTRUCT pCreateStruct);
	void OnDestroy();
	void OnClose();
		
	// Real drawing is done here.
	void Draw(HDC hdc, CRect rcPaint);

	// helpers methods
	HWND Create(HWND hWndParent);
	inline void RedrawWindow() {::RedrawWindow(m_hWnd, 0, 0, RDW_INVALIDATE);}
	void Launch(UINT someparam);
	static CWnd * tWnd;


private:
	
	CRunExWnd() {}
	~CRunExWnd() {}

	void UpdateCntrl();

	void set_selection(metadb_handle_list_cref p_items);

	// message_filter methods
	virtual bool pretranslate_message(MSG * p_msg);

	Utils::PWndSubclasser _pS1;
	HWND hwndToolbar;	
	Utils::PWndProcHook p;
	static CRunExWnd g_instance;

	CFont * m_font;
};



#endif

