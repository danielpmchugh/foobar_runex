#include "stdafx.h"
#include "RunExWnd.h"
#include <afxcmn.h>
#include "preferences.h"
#include "ContextMenuSub.h"


CRunExWnd CRunExWnd::g_instance;
HWND CRunExWnd::hToolbar = NULL;
CToolBar CRunExWnd::m_FirstToolBar;
CWnd * CRunExWnd::tWnd = NULL;
CWnd * CRunExWnd::contextWnd = NULL;
HWND CRunExWnd::hCmpWnd = NULL;


// helper function to get the font used for message boxes as uLOGFONT structure
static t_font_description get_def_font()
{
	NONCLIENTMETRICS ncmetrics = { 0 };
	ncmetrics.cbSize = sizeof(ncmetrics);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncmetrics), &ncmetrics, 0);
	return t_font_description::g_from_font(CreateFontIndirect(&ncmetrics.lfMessageFont));
}


// Stores the font used for drawing text on the window.
static const GUID guid_cfg_font = { 0x44983f90, 0x7632, 0x4a3f, { 0x8d, 0x2a, 0xe4, 0x6e, 0x7a, 0xdc, 0x8f, 0x9 } };
cfg_struct_t<t_font_description> cfg_font(guid_cfg_font, get_def_font());

void CRunExWnd::ShowWindow() {
	g_instance.Create(core_api::get_main_window());
}

void CRunExWnd::HideWindow() {
	// Destroy the window.
	g_instance.Destroy();
}
#include <strsafe.h>

HWND CRunExWnd::Create(HWND p_hWndParent) {

	CWnd * cWnd = CWnd::FromHandle(core_api::get_main_window());
	// Initialize common controls.
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC   = ICC_COOL_CLASSES | ICC_BAR_CLASSES;
	InitCommonControlsEx(&icex);

	HWND hwndOwner = cWnd->GetSafeHwnd();
	tWnd = cWnd->FindWindowExW(hwndOwner,NULL,L"ATL:ReBarWindow32",NULL);
	
	hwndToolbar = tWnd->GetSafeHwnd();
	// Create the rebar.
	
	hCmpWnd = super::Create(core_api::get_main_window(),
		TEXT("RunExWnd"),
		//WS_POPUP | WS_THICKFRAME | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU 
		NULL,
		WS_EX_TRANSPARENT,
		CW_USEDEFAULT, CW_USEDEFAULT, 200, 200);

	CWnd *aWnd = CWnd::FromHandle(hCmpWnd);
	aWnd->ShowWindow(SW_HIDE);

	m_FirstToolBar.CreateEx(CWnd::FromHandle(hCmpWnd), TBSTYLE_FLAT | TBSTYLE_TRANSPARENT );
	m_FirstToolBar.LoadToolBar(IDR_TOOLBAR1  );
	m_FirstToolBar.SetButtonText(0, L"Launch");
	m_FirstToolBar.SetButtonStyle (0, BS_PUSHBUTTON | BS_TOP | BS_TEXT);
	
    console::formatter() << "Created toolbar button" ;

	CReBar tmp;
	REBARBANDINFO rbi = {0};
	rbi.cbSize = 	tmp.GetReBarBandInfoSize(); 
	
	if (SendMessage(hwndToolbar, RB_GETBANDINFO, (WPARAM)0, (LPARAM)&rbi) == 0)
	    console::formatter() << "Failed to get BandInfo" ;
	else
	{
		rbi.fMask = RBBIM_BACKGROUND | RBBIM_CHILD | RBBIM_CHILDSIZE | 
		RBBIM_STYLE | RBBIM_TEXT;
		rbi.fStyle = RBBS_GRIPPERALWAYS| RBBS_CHILDEDGE;
		rbi.cxMinChild = 60;
		rbi.lpText = _T("");
		rbi.cx = 0;
		rbi.hwndChild = m_FirstToolBar.GetSafeHwnd();
		hToolbar = hCmpWnd;
	
		if (SendMessage(hwndToolbar, RB_INSERTBAND, (WPARAM)1, (LPARAM)&rbi) == 0 )
			console::formatter() << "Failed to insert band";
		else
			console::formatter() << "Band inserted";

		CToolBarCtrl& bar = m_FirstToolBar.GetToolBarCtrl();			
		CImageList *pList = bar.GetImageList();
		CBitmap bmp;
		bmp.LoadMappedBitmap(IDB_BITMAP2);
		int index = pList->Add(&bmp,(CBitmap*)(NULL));
 		bar.SetImageList(pList);
	
		CRect temp;			
		m_FirstToolBar.GetItemRect(0,&temp);
		m_FirstToolBar.SetSizes(CSize(32+7,32+6),CSize(32,32));
		m_FirstToolBar.SetButtonText(0,L"");
		m_FirstToolBar.SetButtonInfo(0,ID_TB1_CMD1, TBBS_BUTTON,index);
		m_FirstToolBar.Invalidate();


		UpdateCntrl ();		
		
	}

	p = new Utils::CWndProcHook();

	

	return hCmpWnd;
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
		UpdateCntrl();		
		break;
	case WM_QUERYUISTATE:
		return UISF_ACTIVE;
		break;	

	case WM_TOOLBAR_SHOWHIDE:
		if (m_FirstToolBar.IsWindowVisible())
			m_FirstToolBar.ShowWindow(SW_HIDE);
		else 
			m_FirstToolBar.ShowWindow(SW_SHOW);
		break;
			
	case WM_CONTEXT_MENU_FB:
		//Try to grab the context menu
		contexthWnd =  FindWindowEx( NULL, NULL, MAKEINTATOM(0x8000), NULL );		
		if (contexthWnd  != NULL)
		{			
			contextWnd = CWnd::FromHandle(contexthWnd);					

			HMENU hMenu =  (HMENU) SendMessage(contexthWnd,MN_GETHMENU, 0, 0);
			if (hMenu) // try to get the menu handle
			{	

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

					pMenu->InsertMenuItemW(12,&mii,1);
					if (m_FirstToolBar.IsWindowVisible())	
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
#ifdef _DEBUG
			console::formatter() << "Context is null reposting message";
#endif
			PostMessage(CRunExWnd::hCmpWnd,WM_CONTEXT_MENU_FB,0,0);

		}				
		
		break;
	
	}

	// The framework will call DefWindowProc() for us.
	return FALSE;
}

LRESULT CRunExWnd::OnCreate(LPCREATESTRUCT pCreateStruct) {
	if (DefWindowProc(m_hWnd, WM_CREATE, 0, (LPARAM)pCreateStruct) != 0) return -1;

	// Initialize the font.
	m_font = CFont::FromHandle(cfg_font.get_value().create());

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

void CRunExWnd::UpdateCntrl()
{
	bool bIsBtnText = CMyPreferences::cfg_IsBtnText;
	std::wstring wBtnText, wImagePath;	
	std::string sText = CMyPreferences::cfg_BtnText;
	CMyPreferences::StringToWString (wBtnText, sText);		
		
	sText = CMyPreferences::cfg_ImageText;	
	CMyPreferences::StringToWString (wImagePath, sText);		
	
	int iCnt = SendMessage(hwndToolbar, RB_GETBANDCOUNT, 0,0);

	if (bIsBtnText) 
	{ // If its text display the text instead of an image in the toolbar
		
		
		CWnd * pWnd = CWnd::FromHandle(core_api::get_main_window());
				
		
		m_FirstToolBar.SetSizes(CSize(32+7,32+6),CSize(1,1));
		m_FirstToolBar.SetButtonInfo(0,ID_TB1_CMD1, TBBS_BUTTON,0);
		m_FirstToolBar.SetButtonText(0, wBtnText.c_str());	
		
		/*NONCLIENTMETRICS a_stMetrics;
		a_stMetrics.cbSize = sizeof( a_stMetrics );
		SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( a_stMetrics ), &a_stMetrics, 0 );
		CFont a_oFont;
		a_oFont.CreateFontIndirect( &a_stMetrics.lfSmCaptionFont );
		m_FirstToolBar.SetFont( &a_oFont );*/

		CSize size = m_FirstToolBar.GetDC()->GetTextExtent(wBtnText.c_str());						
		//Todo index can change
		SendMessage(hwndToolbar, RB_SETBANDWIDTH, (WPARAM)1, (LPARAM)size.cx);
	}
	else
	{
		HICON hIcon = NULL;
		// Try to Load the image from file-
		std::wstring ext= wImagePath.substr(wImagePath.length()-4,4);
		for (int i = 0; i < ext.length(); i++)
			ext[i] =toupper(ext[i]);
		if (ext.compare(L".ICO")==0)
		{
			hIcon = (HICON )::LoadImage(NULL, wImagePath.c_str(),
				IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
		}
		if (ext.compare(L".BMP")==0)
		{
			
			HBITMAP bmp = (HBITMAP )::LoadImage(NULL, wImagePath.c_str(),
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

			BITMAP bm;  
			GetObject(bmp, sizeof(BITMAP), &bm);

			CBitmap * cBmp = CBitmap::FromHandle (bmp);
			HBITMAP hbmMask = ::CreateCompatibleBitmap(::GetDC(NULL), 
				bm.bmWidth, bm.bmHeight);
			
			ICONINFO ii = {0};
			ii.fIcon    = TRUE;
			ii.hbmColor = bmp;
			ii.hbmMask  = hbmMask;

			hIcon = ::CreateIconIndirect(&ii);
			::DeleteObject(hbmMask);	
		}
		if (ext.compare(L".CUR")==0)
		{
			HCURSOR bmp = (HCURSOR)::LoadImage(NULL, wImagePath.c_str(),
				IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
			ICONINFO info = {0};
			SIZE res = {0};
			if ( ::GetIconInfo(bmp, &info)!=0 )
			{
				hIcon = ::CreateIconIndirect(&info);
			}			
		}
			
		// if LoadImage fails, it returns a NULL handle
		if(NULL == hIcon)
		{
		  // LoadImage faled so get extended error information.
		  DWORD dwError = ::GetLastError();	  	  
		  std::string ErrMsg = "Unable to load image into RunEx button. Image Path= ";			  
		  ErrMsg+= sText.c_str();
		  console::formatter() << ErrMsg.c_str() << ".";
		  ErrMsg= " Error Code=";	  
		  console::formatter() << ErrMsg.c_str() << (t_int32)dwError  << ".";	  	  		  
		}
		else
		{
			m_FirstToolBar.SetSizes(CSize(32+7,32+6),CSize(32,32));
			
			CToolBarCtrl& bar = m_FirstToolBar.GetToolBarCtrl();
			CImageList *pList = bar.GetImageList();
 			int index = pList->Add(hIcon);			
			bar.SetImageList(pList);
			CRect temp;			
			m_FirstToolBar.GetItemRect(0,&temp);			
			m_FirstToolBar.SetButtonText(0,L"");
			m_FirstToolBar.SetButtonInfo(0,ID_TB1_CMD1, TBBS_BUTTON,index);
			m_FirstToolBar.Invalidate();

		}

	}
	for (int i = 0; i < iCnt; i++)			
	{
		SendMessage(hwndToolbar, RB_SHOWBAND , i,1);			
	}
}
