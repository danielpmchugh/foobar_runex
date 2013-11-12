#pragma once

#include "window_helper.h"
#include <afxext.h>

// This class implements our window. 
// It uses a helper class from window_helper.h that emulates
// ATL/WTL conventions. The custom helper class is used to
// allow this tutorial to be compiled when ATL/WTL is not
// available, for example on Visual C++ 2008 Express Edition.
// The message_filter is used to process keyboard shortcuts.
// To be notified about playback status changes, we need a play
// callback. Those callbacks ar registered and unregistered in
// foobar2000 0.9. Since all callback methods are guaranteed to
// be called in the context of the main thread, we can derive
// our window class from play_callback and register 'this'.




class CToolbarBarRunExe : public ui_element_instance, public CWindowImpl<CToolbarBarRunExe >
{
public:

	// ATL window class declaration. Replace class name with your own when reusing code.\r
	DECLARE_WND_CLASS_EX(TEXT("{DC2917D5-1288-4434-A28C-F16CFCE13C4B}"),CS_VREDRAW | CS_HREDRAW,(-1));

	void initialize_window(HWND parent); 

	BEGIN_MSG_MAP(ui_element_dummy)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDown);
		MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT,OnPaint);
	END_MSG_MAP()

	GUID get_guid() { return g_get_guid(); }
	GUID get_subclass() { return g_get_subclass(); }
	int FB2KAPI service_release() throw() { return 1; }
	int FB2KAPI service_add_ref() throw() { return 1; }

	CToolbarBarRunExe(ui_element_config::ptr, ui_element_instance_callback_ptr p_callback);
	HWND get_wnd() {return *this;}
	void set_configuration(ui_element_config::ptr config) {m_config = config;}
	ui_element_config::ptr get_configuration() {return m_config;}
	static GUID g_get_guid() {
		// This is our GUID. Substitute with your own when reusing code.
		static const GUID guid_myelem = { 0xb46dc166, 0x88f3, 0x4b45, { 0x9f, 0x77, 0xab, 0x33, 0xf4, 0xc3, 0xf2, 0xe4 } };
		return guid_myelem;
	}
	static GUID g_get_subclass() {return ui_element_subclass_utility;}
	static void g_get_name(pfc::string_base & out) {out = "Sample UI Element";}
	static ui_element_config::ptr g_get_default_configuration() {return ui_element_config::g_create_empty(g_get_guid());}
	static const char * g_get_description() {return "This is a sample UI Element.";}
	
	void notify(const GUID & p_what, t_size p_param1, const void * p_param2, t_size p_param2size);

	int UpdateCntrl();
	
	CToolBar toolbar;// WOULD LIKE TO HIDE THIS
	HWND GetToolbarHwnd() { return toolbar.GetSafeHwnd(); }
	
	HICON CreateIcon(CSize * pSize, int maxCY);

private:
	LRESULT OnLButtonDown(UINT,WPARAM,LPARAM,BOOL&) {m_callback->request_replace(this);return 0;}
	LRESULT OnPaint(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);	

	ui_element_config::ptr m_config;
	
protected:
	// this must be declared as protected for ui_element_impl_withpopup<> to work.
	static const GUID s_guid;
	const ui_element_instance_callback_ptr m_callback;

	
};

#if 0

	static void ShowWindow(HWND hwnd);
	static void HideWindow();

	// Dispatches window messages to the appropriate handler functions.
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & lResult);

	// Message handler functions.
	// The function signatures are intended to be compatible with the MSG_WM_* macros in WTL.
	LRESULT OnCreate(LPCREATESTRUCT pCreateStruct);
	void OnDestroy();
	void OnClose();
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnContextMenu(HWND hWnd, CPoint point);
	void OnSetFocus(HWND hWndOld);
	void OnPaint(HDC hdc);
	void OnPrintClient(HDC hdc, UINT uFlags);

	// Helper to handle double buffering when appropriate.
	void PaintContent(PAINTSTRUCT &ps);

	// Real drawing is done here.
	void Draw(HDC hdc, CRect rcPaint);

	// helpers methods
	HWND Create(HWND hWndParent);
	inline void RedrawWindow() {::RedrawWindow(m_hWnd, 0, 0, RDW_INVALIDATE);}

private:
	// This is a singleton class.
	CTutorialWindow() {}
	~CTutorialWindow() {}
		
	void UpdateCntrl();

	CToolBar m_FirstToolBar;

	static bool cfg_enabled;

	static CTutorialWindow g_instance;

	void set_selection(metadb_handle_list_cref p_items);

	// message_filter methods
	virtual bool pretranslate_message(MSG * p_msg);

	// play_callback methods (the ones we're interested in)
	virtual void on_playback_new_track(metadb_handle_ptr p_track);
	virtual void on_playback_stop(play_control::t_stop_reason reason);
	virtual void on_playback_dynamic_info_track(const file_info & p_info);

	// play_callback methods (the rest)
	virtual void on_playback_starting(play_control::t_track_command p_command, bool p_paused) {}
	virtual void on_playback_seek(double p_time) {}
	virtual void on_playback_pause(bool p_state) {}
	virtual void on_playback_edited(metadb_handle_ptr p_track) {}
	virtual void on_playback_dynamic_info(const file_info & p_info) {}
	virtual void on_playback_time(double p_time) {}
	virtual void on_volume_change(float p_new_val) {}

	void ResizeBand (int cx);
	HICON CreateIcon (CSize * pSize, int maxCY);

private:
	CFont m_font;
	HWND hwndToolbar;

	// This is used to notify other components of the selection
	// in our window. In this overly simplistic case, our selection
	// will be empty, when playback is stopped. Otherwise it will
	// contain the playing track.
	ui_selection_holder::ptr m_selection;
};
#endif