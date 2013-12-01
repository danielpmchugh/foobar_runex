#include "stdafx.h"
#include <afxcmn.h>
#include <afxext.h>

#include "preferences.h"
#include "ToolbarBtn.h"
#include "ContextMenuSub.h"
#include "RunExWnd.h"


static service_factory_single_t<ui_element_myimpl> g_ui_element_myimpl_factory;


CToolbarBarRunExe::CToolbarBarRunExe( ) 
{

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
	GetWindowRect(&rect); // Get the size of the toolbar
	
	int iMaxCY = rect.bottom - rect.top - 7;
	int iDesiredCX = 25;
	hIcon = CreateIcon(&size, iDesiredCX, iMaxCY);
			
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
			size.cx = iDesiredCX;
			size.cy = iMaxCY;
		}

		CImageList *pList;
		CToolBarCtrl& bar = GetToolBarCtrl();

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

		SetButtonInfo(0, ID_TB1_CMD1, TBBS_BUTTON, index);
		Invalidate();
	}

	return size.cx;
}


HICON CToolbarBarRunExe::CreateIcon(CSize * pSize, int desiredCX, int desiredCY)
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
			hIcon = (HICON)::LoadImage(NULL, wImagePath.c_str(), IMAGE_ICON, desiredCX, desiredCY, LR_LOADFROMFILE);
		}
		if (ext.compare(L".BMP") == 0)
		{
			HBITMAP bmp = (HBITMAP)::LoadImage(NULL, wImagePath.c_str(), IMAGE_BITMAP, desiredCX, desiredCY, LR_LOADFROMFILE);

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
				IMAGE_CURSOR, desiredCX, desiredCY, LR_LOADFROMFILE);
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
		size.cy = desiredCY;
		bitmap.CreateCompatibleBitmap(pDC, size.cx, size.cy);

		pOldBmp = MemDC.SelectObject(&bitmap);

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

	CreateEx(CWnd::FromHandle(parent), TBSTYLE_FLAT | TBSTYLE_TRANSPARENT);
	LoadToolBar(IDR_TOOLBAR1);
	SetButtonStyle(0, BS_PUSHBUTTON | BS_BITMAP);



	ui_element_instance_callback_ptr p_callback;

	static ui_element_config::ptr ptr_cfg = CMyElemWindowB::g_get_default_configuration();
	static ui_element_myinstance_callback callback;

	ui_element_myimpl &ptr = g_ui_element_myimpl_factory.get_static_instance();
	static service_ptr_t<ui_element_instance> item = ptr.instantiate(GetSafeHwnd(), ptr_cfg, &callback);

	t_ui_font t = callback.query_font_ex(ui_font_default);

	console::formatter() << "Created toolbar button";

}


CMyElemWindowB::CMyElemWindowB(ui_element_config::ptr config, ui_element_instance_callback_ptr p_callback) : m_callback(p_callback), m_config(config) {
}