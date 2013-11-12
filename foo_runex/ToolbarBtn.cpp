#include "stdafx.h"
#include <afxcmn.h>
#include "preferences.h"
#include "ToolbarBtn.h"
#include "ContextMenuSub.h"
#include "RunExWnd.h"

const GUID CToolbarBarRunExe::s_guid = { 0xc9e9885f, 0x2c8a, 0x497f, { 0x88, 0xf0, 0x74, 0x70, 0xd9, 0x20, 0x59, 0xe4 } };

template<typename TImpl>
class my_ui_element_impl : public ui_element
{
public:
	GUID get_guid() { return TImpl::g_get_guid(); }
	GUID get_subclass() { return TImpl::g_get_subclass(); }
	void get_name(pfc::string_base & out) { TImpl::g_get_name(out); }

	ui_element_instance::ptr instantiate(HWND parent, ui_element_config::ptr cfg, ui_element_instance_callback::ptr callback)
	{
		PFC_ASSERT(cfg->get_guid() == get_guid());
		service_nnptr_t<ui_element_instance_impl_helper> item = new service_impl_t<ui_element_instance_impl_helper>(cfg, callback);
		item->initialize_window(parent);
		return item;
	}

	ui_element_config::ptr get_default_configuration() { return TImpl::g_get_default_configuration(); }
	ui_element_children_enumerator_ptr enumerate_children(ui_element_config::ptr cfg) { return NULL; }
	bool get_description(pfc::string_base & out) { out = TImpl::g_get_description(); return true; }

private:
	class ui_element_instance_impl_helper : public TImpl
	{
	public:
		ui_element_instance_impl_helper(ui_element_config::ptr cfg, ui_element_instance_callback::ptr callback)
			: TImpl(cfg, callback) {}
	};
};


//class ui_element_myimpl : public ui_element_impl<CToolbarBarRunExe> {};

//static service_factory_t<ui_element_impl<CToolbarBarRunExe>> g_ui_element_myimpl_factory;


static service_factory_single_t<my_ui_element_impl<CToolbarBarRunExe> > g_ui_element_myimpl_factory;

//g_ui_element_myimpl_factory.instance_create();



void CToolbarBarRunExe::notify(const GUID & p_what, t_size p_param1, const void * p_param2, t_size p_param2size) {
	if (p_what == ui_element_notify_colors_changed || p_what == ui_element_notify_font_changed) {
		// we use global colors and fonts - trigger a repaint whenever these change.
		Invalidate();
	}
}
CToolbarBarRunExe::CToolbarBarRunExe(ui_element_config::ptr config, ui_element_instance_callback_ptr p_callback) : m_callback(p_callback), m_config(config)
{

}

LRESULT CToolbarBarRunExe::OnPaint(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,
      BOOL& /*bHandled*/)
   {
      return 0;   
   }

LRESULT CToolbarBarRunExe::OnEraseBkgnd(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,
      BOOL& /*bHandled*/)
{
      return 0;   
}

int CToolbarBarRunExe::UpdateCntrl()
{
	HICON hIcon = NULL;	
	bool bIsBtnText = CMyPreferences::cfg_IsBtnText;

	std::wstring wImagePath;
	std::string sText = CMyPreferences::cfg_ImageText;
	CMyPreferences::StringToWString(wImagePath, sText);
	

	CSize size;

	RECT rect;
	toolbar.GetWindowRect(&rect); // Get the size of the toolbar

	int iMaxCY = rect.bottom - rect.top - 7;
	hIcon = CreateIcon(&size, iMaxCY);
	
		
	// if LoadImage fails, it returns a NULL handle
	if (NULL == hIcon)
	{
		// LoadImage faled so get extended error information.
		DWORD dwError = ::GetLastError();
		std::string ErrMsg = "Unable to load image into RunEx button. Image Path= ";
		ErrMsg += sText.c_str();
		console::formatter() << ErrMsg.c_str() << ".";
		ErrMsg = " Error Code=";
		console::formatter() << ErrMsg.c_str() << (t_int32)dwError << ".";
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
		CToolBarCtrl& bar = toolbar.GetToolBarCtrl();

		if (iconInfo.hbmColor == NULL)
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
		toolbar.SetButtonInfo(0, ID_TB1_CMD1, TBBS_BUTTON, index);
		toolbar.Invalidate();
		//ResizeBand(size.cx + 4);
	}

	return size.cx;
	
}


HICON CToolbarBarRunExe::CreateIcon(CSize * pSize, int maxCY)
{
		
	HICON hIcon = NULL;
	bool bIsBtnText = CMyPreferences::cfg_IsBtnText;

	std::wstring wImagePath;
	std::string sText = CMyPreferences::cfg_ImageText;
	CMyPreferences::StringToWString(wImagePath, sText);

	std::wstring wBtnText;
	sText = CMyPreferences::cfg_BtnText;
	if (strcmp(sText.c_str(), "") == 0)
	{
		sText = "RunExe";
	}

	CMyPreferences::StringToWString(wBtnText, sText);


	if (!bIsBtnText)
	{
		// Try to Load the image from file-
		std::wstring ext = wImagePath.substr(wImagePath.length() - 4, 4);
		for (int i = 0; i < ext.length(); i++)
			ext[i] = toupper(ext[i]);

		if (ext.compare(L".ICO") == 0)
		{
			hIcon = (HICON)::LoadImage(NULL, wImagePath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
		}
		if (ext.compare(L".BMP") == 0)
		{
			HBITMAP bmp = (HBITMAP)::LoadImage(NULL, wImagePath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

			BITMAP bm;
			GetObject(bmp, sizeof(BITMAP), &bm);

			CBitmap * cBmp = CBitmap::FromHandle(bmp);
			HBITMAP hbmMask = ::CreateCompatibleBitmap(::GetDC(NULL),
				bm.bmWidth, bm.bmHeight);

			ICONINFO ii = { 0 };
			ii.fIcon = TRUE;
			ii.hbmColor = bmp;
			ii.hbmMask = hbmMask;

			hIcon = ::CreateIconIndirect(&ii);
			::DeleteObject(hbmMask);
		}
		if (ext.compare(L".CUR") == 0)
		{
			HCURSOR cur = (HCURSOR)::LoadImage(NULL, wImagePath.c_str(),
				IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
			ICONINFO info = { 0 };
			SIZE res = { 0 };
			if (::GetIconInfo(cur, &info) != 0)
			{
				hIcon = ::CreateIconIndirect(&info);
			}
		}

	}
	else
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

//		CBrush brush;
	//	brush.CreateSolidBrush(RGB(255, 0, 0));
	//	MemDC.SelectObject(&brush);

		CRect rect;
		rect.SetRect(0, 0, size.cx, size.cy);

		MemDC.SetBkMode(TRANSPARENT);
		MemDC.FillSolidRect(0, 0, size.cx, size.cy, pDC->GetBkColor());
		MemDC.DrawText(wBtnText.c_str(), wBtnText.length(), &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		MemDC.SetTextColor(RGB(0, 0, 255));

		//When done, than:
		MemDC.SelectObject(pOldBmp);
		//		ReleaseDC(&MemDC);
		//	ReleaseDC(pDC);

		HBITMAP hbmMask = ::CreateCompatibleBitmap(::GetDC(NULL), size.cx, size.cy);

		ICONINFO ii = { 0 };
		ii.fIcon = TRUE;
		ii.hbmColor = (HBITMAP)bitmap.m_hObject;
		ii.hbmMask = hbmMask;

		hIcon = ::CreateIconIndirect(&ii);
		::DeleteObject(hbmMask);

	}


	ICONINFO info = { 0 };
	if (::GetIconInfo(hIcon, &info) != 0)
	{
		bool bBWCursor = (info.hbmColor == NULL);
		BITMAP bmpinfo = { 0 };
		if (::GetObject(info.hbmMask, sizeof(BITMAP), &bmpinfo) != 0)
		{
			pSize->cx = bmpinfo.bmWidth;
			pSize->cy = abs(bmpinfo.bmHeight) / (bBWCursor ? 2 : 1);
		}

		::DeleteObject(info.hbmColor);
		::DeleteObject(info.hbmMask);
	}
	return hIcon;
}


void CToolbarBarRunExe::initialize_window(HWND parent)
{
	WIN32_OP(Create(parent, 0, 0, 0, WS_EX_STATICEDGE) != NULL);

	toolbar.CreateEx(CWnd::FromHandle(parent), TBSTYLE_FLAT | TBSTYLE_TRANSPARENT);
	toolbar.LoadToolBar(IDR_TOOLBAR1);
	toolbar.SetButtonStyle(0, BS_PUSHBUTTON | BS_BITMAP);

	console::formatter() << "Created toolbar button";


}