#include "stdafx.h"
#include "RunExWnd.h"
#include <afxcmn.h>
#include "preferences.h"
#include "ContextMenuSub.h"
#include <strsafe.h>


const GUID CRunExWnd::guid_cfg_iPos   = { 0x81154312, 0x8df3, 0x4674, { 0x84, 0x36, 0xf1, 0x81, 0x37, 0xc7, 0x34, 0x15 } };
const GUID CRunExWnd::guid_cfg_iVis   = { 0x81154315, 0x8df3, 0x4674, { 0x84, 0x36, 0xf1, 0x81, 0x37, 0xc7, 0x34, 0x11 } };
const GUID CRunExWnd::guid_cfg_iStyle = { 0x81154311, 0x8be3, 0x1674, { 0x84, 0x36, 0xf1, 0x81, 0x37, 0xc7, 0x34, 0x18 } };

cfg_int  CRunExWnd::iPos	 (guid_cfg_iPos, 1);
cfg_bool CRunExWnd::isVisible(guid_cfg_iVis, false);
cfg_int  CRunExWnd::iStyle	 (guid_cfg_iStyle, RBBS_GRIPPERALWAYS | RBBS_CHILDEDGE);

CWnd * CRunExWnd::contextWnd = NULL;
HWND CRunExWnd::hWnd = NULL;
HWND CRunExWnd::hwndRebar = NULL;

void CRunExWnd::ShowWindow() {
	console::formatter() << "Band is visible=" << isVisible;
	console::formatter() << "Band Position is=" << iPos;
	console::formatter() << "Band Style is=" << iStyle;

}

void CRunExWnd::HideWindow() 
{
	
}

void CRunExWnd::SaveConfig()
{
	CReBar tmp;
	REBARBANDINFO rbi = { 0 };
	rbi.cbSize = tmp.GetReBarBandInfoSize();
	rbi.fMask = RBBIM_STYLE;
	GetBandPosition();
	if (SendMessage(hwndRebar, RB_GETBANDINFO, (WPARAM)iPos, (LPARAM)&rbi) != 0)
	{
		if (rbi.fStyle & RBBS_NOGRIPPER)
		{
			rbi.fStyle = rbi.fStyle & (RBBS_GRIPPERALWAYS ^ 0xFFFFFFFF); // TURN OFF RBBS_GRIPPERALWAYS
		}
		else
		{
			rbi.fStyle = rbi.fStyle | RBBS_GRIPPERALWAYS; 
		}
	}
	iStyle = rbi.fStyle;

	SendMessage(hwndRebar, RB_DELETEBAND, (WPARAM)iPos, 0);


}

CWnd * CRunExWnd::GetRebar()
{
	// Get CWnd and HWND for main window
	CWnd * cWndMain = CWnd::FromHandle(core_api::get_main_window());
	HWND hwndMain = cWndMain->GetSafeHwnd();
	
	CWnd * cRebar = NULL;
	// Get CWnd and HWND for Rebar control
	cRebar = cWndMain->FindWindowExW(hwndMain, NULL, L"ReBarWindow32", NULL);
	if (cRebar == NULL)
		cRebar = cWndMain->FindWindowExW(hwndMain, NULL, L"ATL:ReBarWindow32", NULL);

	hwndRebar = cRebar->GetSafeHwnd();
	return cRebar;

}

HWND CRunExWnd::Create(HWND p_hWndParent) 
{	
	CWnd * cWndRebar = GetRebar();
	// Create this window / hidden.	
	hWnd = super::Create(core_api::get_main_window(),
		TEXT("RunExWnd"),		
		NULL,
		WS_EX_TRANSPARENT,
		CW_USEDEFAULT, CW_USEDEFAULT, 200, 200);
	CWnd *cWnd = CWnd::FromHandle(hWnd);
	cWnd->ShowWindow(SW_HIDE);
	
	// Initialize Tool bar control
	tbRunExe.initialize_window(hWnd);
		
	// Create a new rebar band with a reference to the CToolBarRunExe 
	CReBar tmp;
	REBARBANDINFO rbi = {0};
	rbi.cbSize = 	tmp.GetReBarBandInfoSize(); 

	if (SendMessage(hwndRebar, RB_GETBANDINFO, (WPARAM)0, (LPARAM)&rbi) == 0)
	    console::formatter() << "Failed to get BandInfo" ;
	else
	{
		rbi.fMask = RBBIM_BACKGROUND | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE ;
		rbi.fStyle = iStyle;
		rbi.cxMinChild = 32;
		rbi.lpText = _T("");
		rbi.cx = 32;
		rbi.hwndChild = tbRunExe.GetSafeHwnd();

	
		if (SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)iPos, (LPARAM)&rbi) == 0)
			console::formatter() << "Failed to insert band";
		else
			console::formatter() << "Band inserted at position "<< iPos ;

		SendMessage(hwndRebar, RB_SHOWBAND, (WPARAM)iPos, (LPARAM)isVisible);
	}
	
	ResizeBand(tbRunExe.UpdateCntrl() + RPAD);

	CreateHook();	
	
	return hWnd;
}


LRESULT CRunExWnd::OnHook(CWPSTRUCT &cwps)
{
	CWnd * tWnd = GetRebar(); 

	//if (cwps.hwnd == tWnd->GetSafeHwnd())
	{
		switch (cwps.message)
		{
		case WM_CONTEXTMENU:
			if (cwps.hwnd == tWnd->GetSafeHwnd())
				SendMessage(this->m_hWnd, WM_CONTEXT_MENU_FB, 0, 0);
			break;
		case WM_NCHITTEST:
		case WM_SETCURSOR:
		case WM_ERASEBKGND:
		case WM_PARENTNOTIFY:
		case WM_MOUSEACTIVATE:
		case WM_PAINT:
		case WM_WINDOWPOSCHANGING:
		case WM_WINDOWPOSCHANGED:
		case WM_SIZE:
		case WM_CHILDACTIVATE:
		case WM_NCPAINT:
		case WM_CAPTURECHANGED:
		case WM_GETDLGCODE:
		case WM_NCCALCSIZE:
		case WM_CTLCOLORSTATIC:
		case WM_PRINTCLIENT:
		case WM_SETTEXT:
		case WM_STYLECHANGING:
		case WM_SETREDRAW:
		case WM_STYLECHANGED:
		case WM_COMMAND:
		case WM_NOTIFY:
		case WM_CTLCOLORDLG: //chec
		case WM_CTLCOLORBTN:
		case WM_GETTEXT:
		case WM_GETTEXTLENGTH:
		case WM_MOVE:
		case WM_USER:
		case WM_GETFONT:
		case 0xB6:
		case 0xBA:
		case 404:
		case RBN_LAYOUTCHANGED:
		case WM_VSCROLL:
		case WM_ENTERIDLE:
		case WM_SYSCOMMAND:
			break;
		case WM_MENUSELECT:
			char buf[12];
			_snprintf_s(buf, 12, 12, "%X", cwps.hwnd);
			char buf2[12];
			_snprintf_s(buf2, 12, 12, "%X", cwps.message);
			console::formatter() << "HWND=" << buf << "MESSAGE=" << buf2 << cwps.lParam << " " << cwps.wParam;
			break;
		}
	}
	return Default();
}



BOOL CRunExWnd::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & lResult) {

	HWND contexthWnd;
	switch (uMsg) {
	case WM_CREATE:		
		lResult = OnCreate(reinterpret_cast<LPCREATESTRUCT>(lParam));		
		return TRUE;		
		break;
	case WM_DESTROY:		
		OnDestroy();
		lResult = 0;		
		return TRUE;		
		break;
	case WM_CLOSE:
		OnClose();
		lResult = 0;
		return TRUE;		
		break;		
	case WM_COMMAND:		
		Launch(0);		
		return TRUE;		
	case WM_UPDATE_CTRL:
		ResizeBand(tbRunExe.UpdateCntrl() + RPAD);
		break;
	case WM_QUERYUISTATE:
		return UISF_ACTIVE;
		break;	

	case WM_TOOLBAR_SHOWHIDE:
		isVisible = !tbRunExe.IsWindowVisible();
		
		if (isVisible) // Just turned visible, put it at the end.
		{
			GetBandPosition();
			int iCnt = SendMessage(hwndRebar, RB_GETBANDCOUNT, 0, 0);
			if (iPos != iCnt-1)
			{
				SendMessage(hwndRebar, RB_MOVEBAND, iPos, iCnt-1);
				GetBandPosition();
			}
		}

		SendMessage(hwndRebar, RB_SHOWBAND, (WPARAM)iPos, (LPARAM)isVisible);
		ResizeBand(tbRunExe.UpdateCntrl() + RPAD);
		break;

	case WM_NOTIFY:
	   switch (((LPNMHDR)lParam)->code) 
	   {// tooltip?
		  case TTN_NEEDTEXT:
		  {
			 LPTOOLTIPTEXT lpToolTipText;
			 // warning: static buffer not threadsafe
			 static char szBuf[MAX_PATH];

			 // Display the tooltip text.
			 lpToolTipText = (LPTOOLTIPTEXT)lParam;
//			 LoadString (hInst, 
	//			lpToolTipText->hdr.idFrom,   // string ID == cmd ID
		//		szBuf,
			//	sizeof (szBuf) / sizeof (szBuf[0]));
				//lpToolTipText->lpszText = szBuf;
			 break;
		  }
	   }
	   break;

			
	case WM_CONTEXT_MENU_FB:
		//Try to grab the context menu
		console::formatter() << "Looking for context menu...";
		contexthWnd =  FindWindowEx( NULL, NULL, MAKEINTATOM(0x8000), NULL );		
		if (contexthWnd  != NULL)
		{			
			console::formatter() << "Context menu found";
			contextWnd = CWnd::FromHandle(contexthWnd);					

			HMENU hMenu =  (HMENU) SendMessage(contexthWnd,MN_GETHMENU, 0, 0);
			if (hMenu) // try to get the menu handle
			{	
				console::formatter() << "Context Menu Handle found";
				// we have the context menu handle
				CMenu * pMenu = CMenu::FromHandle(hMenu);			
				
				// Add the RunExe button to the context menu
				if (pMenu)
				{
					MENUITEMINFO mii;
				
					mii.cbSize = sizeof(MENUITEMINFO);	
					mii.fMask = MIIM_STRING;
					mii.fType = MFT_STRING;
					mii.fState = MFS_ENABLED; 					
					mii.wID = ID_CONTEXT_MENU_RUN_EXE;
					mii.hSubMenu = NULL;
					mii.hbmpChecked = NULL;
					mii.hbmpUnchecked = NULL;
					mii.dwItemData = NULL;
					mii.dwTypeData = _T("RunEx");
					mii.cch = strlenT(_T("RunEx"));
					mii.hbmpItem = NULL;

					console::formatter() << "Inserting menu item";

					pMenu->InsertMenuItemW(12,&mii,1);
					if (tbRunExe.IsWindowVisible())
						pMenu->CheckMenuItem(12, MF_BYPOSITION|MF_CHECKED   );
																			
				}
							
				// Subclass the menu window allowing us to be notified when menu button is clicked.
					
				_pS1 = new CContextMenuSub(495, 12, WM_TOOLBAR_SHOWHIDE, this->m_hWnd) ;
				_pS1->Subclass(contexthWnd);				
			}
		}
		else
		{
			Sleep(100);			
//#ifdef _DEBUG
			console::formatter() << "Could not find Context menu...checking in 100 ms";
//#endif
			PostMessage(this->m_hWnd,WM_CONTEXT_MENU_FB,0,0);
		}				
		
		break;
	
	}

	// The framework will call DefWindowProc() for us.
	return FALSE;
}

LRESULT CRunExWnd::OnCreate(LPCREATESTRUCT pCreateStruct) {
	if (DefWindowProc(m_hWnd, WM_CREATE, 0, (LPARAM)pCreateStruct) != 0) return -1;

	
	return 0;
}

void CRunExWnd::OnDestroy() {
	
	static_api_ptr_t<message_loop>()->remove_message_filter(this);

	// Notify the window placement variable that our window
	// was destroyed. This will also update the variables value
	// with the current window position and size.
	//cfg_popup_window_placement.on_window_destruction(m_hWnd);
}

bool CRunExWnd::pretranslate_message(MSG * p_msg) {
	// Process keyboard shortcuts
	if (static_api_ptr_t<keyboard_shortcut_manager_v2>()->pretranslate_message(p_msg, m_hWnd)) return true;

	// If you use a dialog or a window with child controls, you can uncomment the following line.
	//if (::IsDialogMessage(m_hWnd, p_msg)) return true;

	return false;
}

void CRunExWnd::OnClose() {
	// Hide and disable the window.
	HideWindow();
}

void CRunExWnd::Launch(UINT p)
{
#ifdef _DEBUG
	console::formatter() << "Current Proc=" << (UINT) GetWindowLongPtr(core_api::get_main_window(), GWL_WNDPROC);
#endif

	STARTUPINFO info={sizeof(info)};
	PROCESS_INFORMATION processInfo;	
	std::wstring cmd;
	CMyPreferences::StringToWString(cmd,CMyPreferences::cfg_Action.get_ptr());
		
	if (CreateProcess(NULL, (LPWSTR)cmd.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo)!=TRUE)
	{
		std::wstring ErrMsg = L"Unable to Launch ";		
		ErrMsg+= cmd;
		AfxMessageBox (ErrMsg.c_str());
	}
}

void CRunExWnd::GetBandPosition()
{
	int iCnt = SendMessage(hwndRebar, RB_GETBANDCOUNT, 0, 0);

	REBARBANDINFO rbi = { 0 };
	rbi.fMask = RBBIM_CHILD;
	CReBar tmp;
	rbi.cbSize = tmp.GetReBarBandInfoSize();


	for (int i = 0; i < iCnt; i++)
	{
		console::formatter() << "Checking Pos " << i;
		if (SendMessage(hwndRebar, RB_GETBANDINFO, (WPARAM)i, (LPARAM)&rbi) != 0)
		{
			if (rbi.hwndChild == tbRunExe.GetSafeHwnd())
			{
				console::formatter() << "Run Exe Found " << i;
				iPos = i;
			}
			else
				console::formatter() << "Run Exe Not Found " << i;

		}
		else
			console::formatter() << "No Band found " << i;
	}

}

void CRunExWnd::ResizeBand (int cx)
{
	CReBar tmp;
	REBARBANDINFO rbi = {0};
	rbi.cbSize = 	tmp.GetReBarBandInfoSize(); 
	
	GetBandPosition();
	if (tbRunExe.IsWindowVisible())
	{	
		if (SendMessage(hwndRebar, RB_GETBANDINFO, (WPARAM)iPos, (LPARAM)&rbi) == 0)
			console::formatter() << "Failed to get BandInfo";
		else
		{
			rbi.fMask = RBBIM_BACKGROUND | RBBIM_CHILD | RBBIM_CHILDSIZE |
				RBBIM_STYLE;
			rbi.fStyle = iStyle;
			rbi.cxMinChild = cx;
			rbi.lpText = _T("");
			rbi.cx = cx;
			rbi.hwndChild = tbRunExe.GetSafeHwnd();

			if (SendMessage(hwndRebar, RB_SETBANDINFO, (WPARAM)iPos, (LPARAM)&rbi) == 0)
				console::formatter() << "Failed to update band";
			else
				console::formatter() << "Band Updated";
		}
	}
	SendMessage(hwndRebar, RB_SHOWBAND, (WPARAM)iPos, (LPARAM)isVisible);
}