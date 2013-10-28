#ifndef WINDOW_HELPER_H
#define WINDOW_HELPER_H
#include "..\helpers\window_placement_helper.h"

//! Special service_impl_t replacement for service classes that also implement ATL/WTL windows.
#if 0
template<typename _parentClass> class CWindowFixSEH : public _parentClass { public:
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) {
		__try {
			return _parentClass::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
		} __except(uExceptFilterProc(GetExceptionInformation())) { return FALSE; /* should not get here */ }
	}
	TEMPLATE_CONSTRUCTOR_FORWARD_FLOOD(CWindowFixSEH, _parentClass);
};


//! Special service_impl_t replacement for service classes that also implement ATL/WTL windows.
template<typename _t_base>
class window_service_impl_t : public CWindowFixSEH<_t_base> {
private:
	typedef window_service_impl_t<_t_base> t_self;
	typedef CWindowFixSEH<_t_base> t_base;
public:
	BEGIN_MSG_MAP(window_service_impl_t)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroyPassThru)	
		CHAIN_MSG_MAP(__super)
	END_MSG_MAP()

	int FB2KAPI service_release() throw() {
		int ret = --m_counter; 
		if (ret == 0) {
			if (window_service_trait_defer_destruction((const preferences_page_instance *)this) && !InterlockedExchange(&m_delayedDestroyInProgress,1)) {
				PFC_ASSERT_NO_EXCEPTION( service_impl_helper::release_object_delayed(this); );
			} else if (m_hWnd != NULL) {
				if (!m_destroyWindowInProgress) { // don't double-destroy in weird scenarios
					PFC_ASSERT_NO_EXCEPTION( ::DestroyWindow(m_hWnd) );
				}
			} else {
				PFC_ASSERT_NO_EXCEPTION( delete this );
			}
		}
		return ret;
	}
	int FB2KAPI service_add_ref() throw() {return ++m_counter;}

	TEMPLATE_CONSTRUCTOR_FORWARD_FLOOD_WITH_INITIALIZER(window_service_impl_t,t_base,{m_destroyWindowInProgress = false; m_delayedDestroyInProgress = 0; })
private:
	LRESULT OnDestroyPassThru(UINT,WPARAM,LPARAM,BOOL&) {
		SetMsgHandled(FALSE); 
		m_destroyWindowInProgress = true;
		return 0;
	}

	void OnFinalMessage(HWND p_wnd) {
		t_base::OnFinalMessage(p_wnd);
		service_ptr_t<service_base> bump(this);
	}
	volatile bool m_destroyWindowInProgress;
	volatile LONG m_delayedDestroyInProgress;
	pfc::refcounter m_counter;
};
#endif
PFC_NORETURN PFC_NOINLINE void WIN32_OP_FAIL();

#ifdef _DEBUG
void WIN32_OP_D_FAIL(const wchar_t * _Message, const wchar_t *_File, unsigned _Line);
#endif






//Throws an exception when (OP) evaluates to false/zero.
#define WIN32_OP(OP)    \
{       \
SetLastError(NO_ERROR); \
if (!(OP)) WIN32_OP_FAIL();     \
}

//WIN32_OP_D() acts like an assert specialized for win32 operations in debug build, ignores the return value / error codes in release build.
//Use WIN32_OP_D() instead of WIN32_OP() on operations that are extremely unlikely to fail, so failure condition checks are performed in the debug build only, to avoid bloating release code with pointless error checks.
#ifdef _DEBUG
#define WIN32_OP_D(OP)  \
{       \
SetLastError(NO_ERROR); \
if (!(OP)) WIN32_OP_D_FAIL(PFC_WIDESTRING(#OP), PFC_WIDESTRING(__FILE__), __LINE__); \
}

#else
#define WIN32_OP_D(OP) (void)( (OP), 0);
#endif

template<typename TDialog> class preferences_page_instance_impl : public TDialog {
public:
	preferences_page_instance_impl(HWND parent, preferences_page_callback::ptr callback) : TDialog(callback) {WIN32_OP(this->Create(parent) != NULL);}
	HWND get_wnd() {return this->m_hWnd;}
};

static bool g_is_enabled()
{
	return standard_config_objects::query_remember_window_positions();
}

static BOOL CALLBACK __MonitorEnumProc(
  HMONITOR hMonitor,  // handle to display monitor
  HDC hdcMonitor,     // handle to monitor DC
  LPRECT lprcMonitor, // monitor intersection rectangle
  LPARAM dwData       // data
  ) {
	RECT * clip = (RECT*)dwData;
	RECT newclip;
	if (UnionRect(&newclip,clip,lprcMonitor)) {
		*clip = newclip;
	}
	return TRUE;
}

static bool test_rect(const RECT * rc) {
	RECT clip = {};
	if (EnumDisplayMonitors(NULL,NULL,__MonitorEnumProc,(LPARAM)&clip)) {
		const LONG sanitycheck = 4;
		const LONG cwidth = clip.right - clip.left;
		const LONG cheight = clip.bottom - clip.top;
		
		const LONG width = rc->right - rc->left;
		const LONG height = rc->bottom - rc->top;

		if (width > cwidth * sanitycheck || height > cheight * sanitycheck) return false;
	}
	
	return MonitorFromRect(rc,MONITOR_DEFAULTTONULL) != NULL;
}



class CMemoryDC : public CDC
{
public:
	HDC m_hDCOriginal;
	CRect m_rcPaint;
	CBitmap * m_bmp;
	HBITMAP m_hBmpOld;

	CMemoryDC(HDC hDC, RECT &rcPaint) : m_hDCOriginal(hDC), m_hBmpOld(NULL) {
		m_rcPaint = rcPaint;
		Attach(::CreateCompatibleDC(m_hDCOriginal));
		m_bmp = CBitmap::FromHandle(::CreateCompatibleBitmap(m_hDCOriginal, m_rcPaint.Width(), m_rcPaint.Height()));
		m_hBmpOld = (HBITMAP)::SelectObject(m_hDC, m_bmp);
		::SetWindowOrgEx(m_hDC, m_rcPaint.left, m_rcPaint.top, NULL);
	}

	~CMemoryDC() {
		::BitBlt(m_hDCOriginal, m_rcPaint.left, m_rcPaint.top, m_rcPaint.Width(), m_rcPaint.Height(), m_hDC, m_rcPaint.left, m_rcPaint.top, SRCCOPY);
		::SelectObject(m_hDC, m_hBmpOld);
	}
};

extern cfg_bool cfg_enabled;

//extern cfg_window_placement cfg_popup_window_placement;

extern cfg_struct_t<t_font_description> cfg_font;

//extern advconfig_string_factory g_advconfig_string_format;


// A light-weight implementation helper for window classes.
template<class T>
class CSimpleWindowImpl
{
private:
	static LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static ATOM g_class_atom;
	static LONG g_refcount;

public:
	HWND m_hWnd;

	CSimpleWindowImpl() {m_hWnd = NULL;}

	HWND Create(HWND hWndParent, LPCTSTR szWindowName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int nWidth, int nHeight, HMENU hMenu = (HMENU)NULL);
	void Destroy();

	inline operator HWND() const {return m_hWnd;}
	inline bool IsWindow() const {return ::IsWindow(m_hWnd) != FALSE;}

	// Override these in an implementation class.
	static LPCTSTR GetClassName();
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & lResult);
};

template <class T>
ATOM CSimpleWindowImpl<T>::g_class_atom = 0;

template <class T>
LONG CSimpleWindowImpl<T>::g_refcount = 0;

template <class T>
LPCTSTR CSimpleWindowImpl<T>::GetClassName() {
	// set up a unique class name using the address of a variable in out class.
	static int dummy = 0;
	static pfc::stringcvt::string_os_from_utf8 os_class_name;
	if (os_class_name.is_empty())
	{
		pfc::string_formatter class_name;
		class_name << "CLS" << pfc::format_hex((t_uint64)&dummy, sizeof(&dummy) * 2);
		os_class_name.convert(class_name);
	}
	return os_class_name;
}

template <class T>
HWND CSimpleWindowImpl<T>::Create(HWND hWndParent, LPCTSTR szWindowName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int nWidth, int nHeight, HMENU hMenu) {
	T *_this = static_cast<T *>(this);

	assert(_this->m_hWnd == NULL);

	if (g_refcount == 0) {
		if (g_class_atom == NULL) {
			WNDCLASS wc = { 0 };
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.lpfnWndProc = &WindowProc;
			wc.hInstance = core_api::get_my_instance();
			wc.hCursor = ::LoadCursor(0, IDC_ARROW);
			wc.hbrBackground = 0;
			wc.lpszClassName = T::GetClassName();
			wc.cbWndExtra = sizeof(CSimpleWindowImpl<T> *);
			g_class_atom = ::RegisterClass(&wc);
		}
	}

	if (g_class_atom != NULL) {
		g_refcount++;

		return ::CreateWindowEx(
			dwExStyle,
			(LPCTSTR)g_class_atom,
			szWindowName,
			dwStyle,
			x, y, nWidth, nHeight,
			hWndParent,
			hMenu,
			core_api::get_my_instance(),
			_this );
	}

	return NULL;
}

template <class T>
void CSimpleWindowImpl<T>::Destroy() {
	// Destroy the window.
	if (m_hWnd) {
		::DestroyWindow(m_hWnd);

		g_refcount--;

		if (g_refcount == 0) {
			::UnregisterClass((LPCTSTR)g_class_atom, core_api::get_my_instance());
			g_class_atom = 0;
		}
	}
}

template <class T>
LRESULT WINAPI CSimpleWindowImpl<T>::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	T *_this;

	if (uMsg == WM_NCCREATE) {
		LPCREATESTRUCT lpCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		_this = reinterpret_cast<T *>(lpCreateStruct->lpCreateParams);
		_this->m_hWnd = hWnd;
		SetWindowLongPtr(hWnd, 0, (LONG_PTR)_this);
	} else {
		 _this = reinterpret_cast<T *>(GetWindowLongPtr(hWnd, 0));
	}

	BOOL bHandled = FALSE;
	LRESULT lResult = 0;
	if (_this != 0) {
		try {
			BOOL bHandled = _this->ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult);
		}
		catch (const std::exception &) {
		}
	}
	if (!bHandled) {
		lResult = DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	if (uMsg == WM_NCDESTROY) {
		_this->m_hWnd = 0;
	}

	return lResult;
}

template <class T>
BOOL CSimpleWindowImpl<T>::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & lResult) {
	lResult = ::DefWindowProc(m_hWnd, msg, wParam, lParam);
	return TRUE;
}




#endif