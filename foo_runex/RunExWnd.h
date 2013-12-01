#ifndef RUNEXWND_H
#define RUNEXWND_H


#include "window_helper.h"
#include "Utility/WndSubclasser.h"
#include "Utility/WndProcHook.h"
#include "ToolbarBtn.h"


class CRunExWnd :
	public CSimpleWindowImpl<CRunExWnd>,
	private message_filter,
	public Utils::CWndProcHook
{
public:
	static CWnd * contextWnd;		
	typedef CSimpleWindowImpl<CRunExWnd> super;
	
	void ShowWindow();
	void HideWindow();
	void SaveConfig();

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
	
	static HWND hWnd;
	CRunExWnd() 
	{
		console::formatter() << "Constructor CRunExWnd";
	}
	~CRunExWnd() 
	{
		console::formatter() << "Deconstructor CRunExWnd";
	}

private:
	
	
	CWnd * GetRebar();
	void ResizeBand (int cx);
	HICON CreateIcon (CSize * pSize, int maxCY);

	void GetBandPosition();
	void set_selection(metadb_handle_list_cref p_items);

	// message_filter methods
	virtual bool pretranslate_message(MSG * p_msg);

	Utils::PWndSubclasser _pS1;
	static HWND hwndRebar;

	LRESULT OnHook(CWPSTRUCT& cwps);
	CFont * m_font;
	const int RPAD = 4;
	static HWND tbHwnd;

	static const GUID guid_cfg_iPos;
	static const GUID guid_cfg_iVis;
	static const GUID guid_cfg_iStyle;

	static cfg_int iPos;
	static cfg_bool isVisible;
	static cfg_int iStyle;

private:
	CToolbarBarRunExe tbRunExe;
};

#endif

