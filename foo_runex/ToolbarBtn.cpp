#include "stdafx.h"
#include <afxcmn.h>
#include "preferences.h"
#include "ToolbarBtn.h"
#include "ContextMenuSub.h"
#include "RunExWnd.h"

const GUID CToolbarBtn::s_guid = { 0xc9e9885f, 0x2c8a, 0x497f, { 0x88, 0xf0, 0x74, 0x70, 0xd9, 0x20, 0x59, 0xe4 } };




void CToolbarBtn::notify(const GUID & p_what, t_size p_param1, const void * p_param2, t_size p_param2size) {
	if (p_what == ui_element_notify_colors_changed || p_what == ui_element_notify_font_changed) {
		// we use global colors and fonts - trigger a repaint whenever these change.
		Invalidate();
	}
}
CToolbarBtn::CToolbarBtn(ui_element_config::ptr config,ui_element_instance_callback_ptr p_callback) : m_callback(p_callback), m_config(config) {

}

LRESULT CToolbarBtn::OnPaint(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, 
      BOOL& /*bHandled*/)
   {
      return 0;   
   }

LRESULT CToolbarBtn::OnEraseBkgnd(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, 
      BOOL& /*bHandled*/)
   {
      return 0;   
   }

/*
BOOL CMyElemWindow::OnEraseBkgnd(CDCHandle dc) {
	CRect rc; WIN32_OP_D( GetClientRect(&rc) );
	CBrush brush;
	WIN32_OP_D( brush.CreateSolidBrush( m_callback->query_std_color(ui_color_background) ) != NULL );
	WIN32_OP_D( dc.FillRect(&rc, brush) );
	return TRUE;
}
void CMyElemWindow::OnPaint(CDCHandle) {
	CPaintDC dc(*this);
	dc.SetTextColor( m_callback->query_std_color(ui_color_text) );
	dc.SetBkMode(TRANSPARENT);
	SelectObjectScope fontScope(dc, (HGDIOBJ) m_callback->query_font_ex(ui_font_default) );
	const UINT format = DT_NOPREFIX | DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	CRect rc; 
	WIN32_OP_D( GetClientRect(&rc) );
	WIN32_OP_D( dc.DrawText(_T("This is a sample element."), -1, &rc, format) > 0 );
}
*/
// ui_element_impl_withpopup autogenerates standalone version of our component and proper menu commands. Use ui_element_impl instead if you don't want that.
class ui_element_myimpl : public ui_element_impl<CToolbarBtn> {};

static service_factory_t<ui_element_impl<CToolbarBtn>> g_ui_element_myimpl_factory;



#if 0

CTutorialWindow CTutorialWindow::g_instance;
bool CTutorialWindow::cfg_enabled = false;

void CTutorialWindow::ShowWindow(HWND hwnd) {
	if (!g_instance.IsWindow()) {
		cfg_enabled = (g_instance.Create(hwnd) != NULL);
	}
}

void CTutorialWindow::HideWindow() {
	// Set window state to disabled.
	cfg_enabled = false;

	// Destroy the window.
	g_instance.Destroy();
}

HWND CTutorialWindow::Create(HWND p_hWndParent) {

	HWND hwnd = super::Create(core_api::get_main_window(),
		TEXT("RunExWndTEST"),		
		NULL,
		WS_EX_TRANSPARENT,
		CW_USEDEFAULT, CW_USEDEFAULT, 200, 200);

	m_FirstToolBar.CreateEx(CWnd::FromHandle(p_hWndParent), TBSTYLE_FLAT | TBSTYLE_TRANSPARENT );
	m_FirstToolBar.LoadToolBar(IDR_TOOLBAR1);	
	m_FirstToolBar.SetButtonStyle (0, BS_PUSHBUTTON | BS_BITMAP );
	
	console::formatter() << "Created toolbar button using BP" ;

	CReBar tmp;
	REBARBANDINFO rbi = {0};
	rbi.cbSize = 	tmp.GetReBarBandInfoSize(); 

	CWnd * cWnd = CWnd::FromHandle(core_api::get_main_window());
	HWND hwndOwner = cWnd->GetSafeHwnd();
	CWnd *  tWnd = cWnd->FindWindowExW(hwndOwner,NULL,L"ATL:ReBarWindow32",NULL);
	
	hwndToolbar = tWnd->GetSafeHwnd();
	
	if (SendMessage(hwndToolbar, RB_GETBANDINFO, (WPARAM)0, (LPARAM)&rbi) == 0)
	    console::formatter() << "Failed to get BandInfo" ;
	else
	{
		rbi.fMask = RBBIM_BACKGROUND | RBBIM_CHILD | RBBIM_CHILDSIZE | 
		RBBIM_STYLE ;
		rbi.fStyle = RBBS_GRIPPERALWAYS| RBBS_CHILDEDGE;
		rbi.cxMinChild = 32;
		rbi.lpText = _T("");
		rbi.cx = 32;
		rbi.hwndChild = m_FirstToolBar.GetSafeHwnd();
		//hToolbar = hCmpWnd;
	
		if (SendMessage(hwndToolbar, RB_INSERTBAND, (WPARAM)1, (LPARAM)&rbi) == 0 )
			console::formatter() << "Failed to insert band";
		else
			console::formatter() << "Band inserted";
		
		UpdateCntrl ();		
		
	}

	return hwnd;
//	CWnd *aWnd = CWnd::FromHandle(hCmpWnd);
//	aWnd->ShowWindow(SW_HIDE);
	
}

BOOL CTutorialWindow::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & lResult) {
	switch (uMsg) {
	case WM_CREATE:
		{
			lResult = OnCreate(reinterpret_cast<LPCREATESTRUCT>(lParam));
			return TRUE;
		}

	case WM_DESTROY:
		{
			OnDestroy();
			lResult = 0;
			return TRUE;
		}

	case WM_CLOSE:
		{
			OnClose();
			lResult = 0;
			return TRUE;
		}

	case WM_KEYDOWN:
		{
			OnKeyDown((TCHAR)wParam, (UINT)lParam & 0xfff, (UINT)((lParam >> 16) & 0xffff));
			lResult = 0;
			return TRUE;
		}

	case WM_LBUTTONDOWN:
		{
			OnLButtonDown(wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
			lResult = 0;
			return TRUE;
		}

	case WM_CONTEXTMENU:
		{
			OnContextMenu((HWND)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
			lResult = 0;
			return TRUE;
		}

	case WM_SETFOCUS:
		{
			OnSetFocus((HWND)wParam);
			lResult = 0;
			return TRUE;
		}

	case WM_PAINT:
		{
			OnPaint((HDC)wParam);
			lResult = 0;
			return TRUE;
		}

	}

	// The framework will call DefWindowProc() for us.
	return FALSE;
}

LRESULT CTutorialWindow::OnCreate(LPCREATESTRUCT pCreateStruct) {
	if (DefWindowProc(m_hWnd, WM_CREATE, 0, (LPARAM)pCreateStruct) != 0) return -1;

	// If "Remember window positions" is enabled, this will
	// restore the last position of our window. Otherwise it
	// won't do anything.
	//cfg_popup_window_placement.on_window_creation(m_hWnd);

	// Initialize the font.
	//m_font = cfg_font.get_value().create();

	// Register ourselves as message_filter, so we can process keyboard shortcuts
	// and (possibly) dialog messages.
	static_api_ptr_t<message_loop>()->add_message_filter(this);

	return 0;
}

void CTutorialWindow::OnDestroy() {
	m_selection.release();

	static_api_ptr_t<message_loop>()->remove_message_filter(this);

	static_api_ptr_t<play_callback_manager>()->unregister_callback(this);

	// Notify the window placement variable that our window
	// was destroyed. This will also update the variables value
	// with the current window position and size.
//	cfg_popup_window_placement.on_window_destruction(m_hWnd);
}

void CTutorialWindow::OnClose() {
	// Hide and disable the window.
	HideWindow();
}

void CTutorialWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (nChar == VK_ESCAPE) {
		// Hide and disable the window.
		HideWindow();
	}
}

void CTutorialWindow::OnLButtonDown(UINT nFlags, CPoint point) {
	// Get currently playing track.
	static_api_ptr_t<play_control> pc;
	metadb_handle_ptr handle;

	// If some track is playing...
	if (pc->get_now_playing(handle)) {
		POINT pt;
		GetCursorPos(&pt);

		// ...detect a drag operation.
		if (DragDetect(m_hWnd, pt)) {
			metadb_handle_list items;
			items.add_item(handle);

			// Create an IDataObject that contains the dragged track.
			static_api_ptr_t<playlist_incoming_item_filter> piif;
			// create_dataobject_ex() returns a smart pointer unlike create_dataobject()
			// which returns a raw COM pointer. The less chance we have to accidentally
			// get the reference counting wrong, the better.
			pfc::com_ptr_t<IDataObject> pDataObject = piif->create_dataobject_ex(items);

			// Create an IDropSource.
			// The constructor of IDropSource_tutorial1 is hidden by design; we use the
			// provided factory method which returns a smart pointer.
		//	pfc::com_ptr_t<IDropSource> pDropSource = IDropSource_tutorial1::g_create(m_hWnd);

			DWORD effect;
			// Perform drag&drop operation.
//			DoDragDrop(pDataObject.get_ptr(), pDropSource.get_ptr(), DROPEFFECT_COPY, &effect);
		}
	}
}

void CTutorialWindow::OnPaint(HDC hdc) {
	CPaintDC dc(CWnd::FromHandle(m_hWnd));
	PaintContent(dc.m_ps);
}


void CTutorialWindow::PaintContent(PAINTSTRUCT &ps) {
	if (GetSystemMetrics(SM_REMOTESESSION)) {
		// Do not use double buffering, if we are running on a Remote Desktop Connection.
		// The system would have to transfer a bitmap everytime our window is painted.
		Draw(ps.hdc, ps.rcPaint);
	} else if (!IsRectEmpty(&ps.rcPaint)) {
		// Use double buffering for local drawing.
		CMemoryDC dc(ps.hdc, ps.rcPaint);
		Draw(dc, ps.rcPaint);
	}
}

void CTutorialWindow::Draw(HDC hdc, CRect rcPaint) {
	// We will paint the background in the default window color.
	HBRUSH hBrush = GetSysColorBrush(COLOR_WINDOW);
	FillRect(hdc, rcPaint, hBrush);

	HICON hIcon = static_api_ptr_t<ui_control>()->get_main_icon();
	if (hIcon != NULL) {
		DrawIconEx(hdc, 2, 2, hIcon, 32, 32, 0, hBrush, DI_NORMAL);
	}

	try
	{
		static_api_ptr_t<play_control> pc;
		metadb_handle_ptr handle;;
		if (pc->get_now_playing(handle)) {
			pfc::string8 format;
//			g_advconfig_string_format.get_static_instance().get_state(format);
			service_ptr_t<titleformat_object> script;
			static_api_ptr_t<titleformat_compiler>()->compile_safe(script, format);

			pfc::string_formatter text;
			pc->playback_format_title_ex(handle, NULL, text, script, NULL, play_control::display_level_titles);
			HFONT hFont = m_font;
			if (hFont == NULL)
				hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			SelectObject(hdc, hFont);
			SetTextAlign(hdc, TA_TOP | TA_LEFT);
			uExtTextOut(hdc, 32+4, 2, ETO_CLIPPED, rcPaint, text, text.length(), 0);
		}
	}
	catch (const std::exception & exc) {
		console::formatter() << "Exception occurred while drawing " << " window:\n" << exc;
	}
}

void CTutorialWindow::OnContextMenu(HWND hWnd, CPoint point) {
	// We need some IDs for the context menu.
	enum {
		// ID for "Choose font..."
		ID_FONT = 1,
		// The range ID_CONTEXT_FIRST through ID_CONTEXT_LAST is reserved
		// for menu entries from menu_manager.
		ID_CONTEXT_FIRST,
		ID_CONTEXT_LAST = ID_CONTEXT_FIRST + 1000,
	};

	// Create new popup menu.
	HMENU hMenu = CreatePopupMenu();

	// Add our "Choose font..." command.
	AppendMenu(hMenu, MF_STRING, ID_FONT, TEXT("Choose font..."));

	// Get the currently playing track.
	metadb_handle_list items;
	static_api_ptr_t<play_control> pc;
	metadb_handle_ptr handle;
	if (pc->get_now_playing(handle)) {
		// Insert it into a list.
		items.add_item(handle);
	}

	// Create a menu_manager that will build the context menu.
	service_ptr_t<contextmenu_manager> cmm;
	contextmenu_manager::g_create(cmm);
	// Query setting for showing keyboard shortcuts.
	const bool show_shortcuts = config_object::g_get_data_bool_simple(standard_config_objects::bool_show_keyboard_shortcuts_in_menus, false);
	// Set up flags for contextmenu_manager::init_context.
	unsigned flags = show_shortcuts ? contextmenu_manager::FLAG_SHOW_SHORTCUTS : 0;
	// Initialize menu_manager for using a context menu.
	cmm->init_context(items, flags);
	// If the menu_manager has found any applicable commands,
	// add them to our menu (after a separator).
	if (cmm->get_root()) {
		uAppendMenu(hMenu, MF_SEPARATOR, 0, 0);
		cmm->win32_build_menu(hMenu, ID_CONTEXT_FIRST, ID_CONTEXT_LAST);
	}

	// Use menu helper to gnereate mnemonics.
	menu_helpers::win32_auto_mnemonics(hMenu);

	// Get the location of the mouse pointer.
	// WM_CONTEXTMENU provides position of mouse pointer in argument lp,
	// but this isn't reliable (for example when the user pressed the
	// context menu key on the keyboard).
	CPoint pt;
//	GetCursorPos(pt);
	// Show the context menu.
	//int cmd = TrackPopupMenu(hMenu, TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON, 
	//	pt.x, pt.y, 0, m_hWnd, 0);

	// Check what command has been chosen. If cmd == 0, then no command
	// was chosen.
//	if (cmd == ID_FONT) {
		// Show font configuration.
	//	t_font_description font = cfg_font;
		//if (font.popup_dialog(m_hWnd)) {
			//cfg_font = font;
//			m_font = font.create();
	//		::RedrawWindow(m_hWnd, 0, 0, RDW_INVALIDATE|RDW_UPDATENOW);
//		}
	//} else if (cmd >= ID_CONTEXT_FIRST && cmd <= ID_CONTEXT_LAST ) {
		//// Let the menu_manager execute the chosen command.
//		cmm->execute_by_id(cmd - ID_CONTEXT_FIRST);
//	}

	// contextmenu_manager instance is released automatically, as is the metadb_handle we used.

	// Finally, destroy the popup menu.
	DestroyMenu(hMenu);
}

void CTutorialWindow::OnSetFocus(HWND hWndOld) {
	metadb_handle_list items;
	metadb_handle_ptr track;
	if (static_api_ptr_t<playback_control>()->get_now_playing(track)) {
		items.add_item(track);
	}
	set_selection(items);
}

void CTutorialWindow::set_selection(metadb_handle_list_cref p_items) {
	// Only notify other components about changes in our selection,
	// if our window is the active one.
	if (::GetFocus() == m_hWnd && m_selection.is_valid()) {
		m_selection->set_selection_ex(p_items, contextmenu_item::caller_now_playing);
	}
}

bool CTutorialWindow::pretranslate_message(MSG * p_msg) {
	// Process keyboard shortcuts
	if (static_api_ptr_t<keyboard_shortcut_manager_v2>()->pretranslate_message(p_msg, m_hWnd)) return true;

	// If you use a dialog or a window with child controls, you can uncomment the following line.
	//if (::IsDialogMessage(m_hWnd, p_msg)) return true;

	return false;
}

void CTutorialWindow::on_playback_new_track(metadb_handle_ptr p_track) {
	RedrawWindow();
	set_selection(pfc::list_single_ref_t<metadb_handle_ptr>(p_track));
}

void CTutorialWindow::on_playback_stop(play_control::t_stop_reason reason) {
	RedrawWindow();
	set_selection(metadb_handle_list());
}

void CTutorialWindow::on_playback_dynamic_info_track(const file_info & p_info) {
	RedrawWindow();
}


void CTutorialWindow::UpdateCntrl()
{	
	std::wstring wImagePath, wBtnText;	
	HICON hIcon = NULL;

	bool bIsBtnText = CMyPreferences::cfg_IsBtnText;	

	std::string sText = CMyPreferences::cfg_ImageText;
	CMyPreferences::StringToWString (wImagePath, sText);		

	sText = CMyPreferences::cfg_BtnText;
	CMyPreferences::StringToWString (wBtnText, sText);
	
	CSize size;

	RECT rect;
	m_FirstToolBar.GetWindowRect(&rect); // Get the size of the toolbar

	int iMaxCY = rect.bottom - rect.top - 7;
			
	hIcon = CreateIcon(&size,iMaxCY);
		
	int iCnt = SendMessage(hwndToolbar, RB_GETBANDCOUNT, 0,0);
		

	// if LoadImage fails, it returns a NULL handle
	if(NULL == hIcon)
	{
		// LoadImage faled so get extended error information.
		DWORD dwError = ::GetLastError();	  	  
		std::string ErrMsg = "Unable to load image into RunEx button. Image Path= " ;			  
		ErrMsg+= sText.c_str();
		console::formatter() << ErrMsg.c_str() << ".";
		ErrMsg= " Error Code=";	  
		console::formatter() << ErrMsg.c_str() << (t_int32)dwError  << ".";	  	  		  
	}
	else
	{
		ICONINFO iconInfo;
		GetIconInfo(hIcon, &iconInfo);
				
		if (!bIsBtnText)
		{
			size.cx = 25;
			size.cy = iMaxCY;
		}
		else
		{
		//	size.cx = max(32,size.cx);
		//	size.cy = max(32,size.cy);
		}

		CImageList *pList;
		CToolBarCtrl& bar = m_FirstToolBar.GetToolBarCtrl();							

		if (iconInfo.hbmColor==NULL)
		{
			pList = bar.GetImageList();
		}
		else
		{
			
			bar.GetImageList()->DeleteImageList();
			pList = new CImageList();
			pList->Create(size.cx, size.cy, ILC_COLOR, 1, 1);
		}
		
	
		int index = pList->Add(hIcon);			
		bar.SetImageList(pList);
		
		
		IMAGEINFO ImageInfo;
		pList->GetImageInfo(index, &ImageInfo);

		//m_FirstToolBar.SetSizes(CSize(20,20),CSize(10,10));		
		m_FirstToolBar.SetButtonInfo(0,ID_TB1_CMD1, TBBS_BUTTON,index);
		m_FirstToolBar.Invalidate();

		SendMessage(hwndToolbar, TB_AUTOSIZE, 0, 0);

		ResizeBand (size.cx+4);
	}

	


	for (int i = 0; i < iCnt; i++)			
	{
		SendMessage(hwndToolbar, RB_SHOWBAND , i,1);			
	}
}


void CTutorialWindow::ResizeBand (int cx)
{
	CReBar tmp;
	REBARBANDINFO rbi = {0};
	rbi.cbSize = 	tmp.GetReBarBandInfoSize(); 

	if (SendMessage(hwndToolbar, RB_GETBANDINFO, (WPARAM)0, (LPARAM)&rbi) == 0)
		console::formatter() << "Failed to get BandInfo" ;
	else
	{
		rbi.fMask = RBBIM_BACKGROUND | RBBIM_CHILD | RBBIM_CHILDSIZE | 
			RBBIM_STYLE ;
		rbi.fStyle = RBBS_GRIPPERALWAYS| RBBS_CHILDEDGE;
		rbi.cxMinChild = cx;
		rbi.lpText = _T("");
		rbi.cx = cx;
		rbi.hwndChild = m_FirstToolBar.GetSafeHwnd();
		//hToolbar = hCmpWnd;

		if (SendMessage(hwndToolbar, RB_SETBANDINFO, (WPARAM)1, (LPARAM)&rbi) == 0 )
			console::formatter() << "Failed to update band";
		else
			console::formatter() << "Band Updated";	
	}
}


HICON CTutorialWindow::CreateIcon (CSize * pSize, int maxCY)
{
	std::wstring wImagePath, wBtnText;	
	HICON hIcon = NULL;

	bool bIsBtnText = CMyPreferences::cfg_IsBtnText;	

	std::string sText = CMyPreferences::cfg_ImageText;
	CMyPreferences::StringToWString (wImagePath, sText);		

	sText = CMyPreferences::cfg_BtnText;
	CMyPreferences::StringToWString (wBtnText, sText);
	
	if (!bIsBtnText) 
	{ 
		// Try to Load the image from file-
		std::wstring ext= wImagePath.substr(wImagePath.length()-4,4);
		for (int i = 0; i < ext.length(); i++)
			ext[i] =toupper(ext[i]);

		if (ext.compare(L".ICO")==0)
		{
			hIcon = (HICON )::LoadImage(NULL, wImagePath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);			
		}
		if (ext.compare(L".BMP")==0)
		{
			HBITMAP bmp = (HBITMAP )::LoadImage(NULL, wImagePath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

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
			HCURSOR cur = (HCURSOR)::LoadImage(NULL, wImagePath.c_str(),
				IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
			ICONINFO info = {0};
			SIZE res = {0};
			if ( ::GetIconInfo(cur, &info)!=0 )
			{
				hIcon = ::CreateIconIndirect(&info);				
			}			
		}

	} else
	{
	
		CBitmap bitmap;
		CBitmap *pOldBmp;
		CDC MemDC;
				
		CWnd * mWnd = CWnd::FromHandle(core_api::get_main_window());

		CDC *pDC = mWnd->GetDC();
		MemDC.CreateCompatibleDC(pDC);
		
		CSize size = MemDC.GetTextExtent(wBtnText.c_str(), wBtnText.length());
		size.cy = maxCY;
		bitmap.CreateCompatibleBitmap(pDC, size.cx, size.cy);
		
		pOldBmp = MemDC.SelectObject(&bitmap);

///		CBrush brush;
//		brush.CreateSolidBrush(RGB(255,0,0));
//		MemDC.SelectObject(&brush);				
		

		CRect rect;
		rect.SetRect (0,0,size.cx,size.cy);
		
//		CFont font;
//		NONCLIENTMETRICS nm;
//		nm.cbSize = sizeof (NONCLIENTMETRICS);
//		SystemParametersInfo (SPI_GETNONCLIENTMETRICS,0,&nm,0);
//		LOGFONT fl = nm.lfMenuFont;
//		fl.lfWeight = FW_BOLD;		
//		LOGFONT LogFont;
//		ZeroMemory(&LogFont,sizeof(LogFont));			
//		CFont * pCustomFont = new CFont();
//		pCustomFont->CreateFontIndirect(&LogFont);
//		MemDC.SelectObject(pCustomFont);

		HBRUSH hBrush = GetSysColorBrush(COLOR_BTNFACE);
		
		MemDC.FillRect(&rect, CBrush::FromHandle(hBrush));

		HFONT hFont = m_font;
		if (hFont == NULL)
			hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		MemDC.SelectObject(hFont);
		

		MemDC.SetBkMode(TRANSPARENT);
		MemDC.FillSolidRect(0,0,size.cx,size.cy, pDC->GetBkColor());
		MemDC.DrawText(wBtnText.c_str(),wBtnText.length(), &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER );		
		MemDC.SetTextColor(RGB(0,0,255));
	
		//When done, than:
		MemDC.SelectObject(pOldBmp);	
//		ReleaseDC(&MemDC);
	//	ReleaseDC(pDC);

		HBITMAP hbmMask = ::CreateCompatibleBitmap(::GetDC(NULL), size.cx,size.cy);

		ICONINFO ii = {0};
		ii.fIcon    = TRUE;
		ii.hbmColor = (HBITMAP)bitmap.m_hObject;
		ii.hbmMask  = hbmMask;

		hIcon = ::CreateIconIndirect(&ii);
		::DeleteObject(hbmMask);	

	}


	ICONINFO info = {0};		
	if ( ::GetIconInfo(hIcon, &info)!=0 )
	{
		bool bBWCursor = (info.hbmColor==NULL);
		BITMAP bmpinfo = {0};
		if (::GetObject( info.hbmMask, sizeof(BITMAP), &bmpinfo)!=0)
		{
			pSize->cx = bmpinfo.bmWidth;
			pSize->cy = abs(bmpinfo.bmHeight) / (bBWCursor ? 2 : 1);
		}

		::DeleteObject(info.hbmColor);
		::DeleteObject(info.hbmMask);
	}
	return hIcon;
}

#endif