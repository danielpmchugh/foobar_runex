#pragma once

#include "window_helper.h"

class CToolbarBarRunExe : public CToolBar
{

public:
	CToolbarBarRunExe();

	void initialize_window(HWND parent);
	
	int UpdateCntrl();

private:
	HICON CreateIcon(CSize * pSize, int desiredCX, int desiredCY);


};


class CMyElemWindowB : public ui_element_instance, public CWindowImpl<CMyElemWindowB> {
public:
	// ATL window class declaration. Replace class name with your own when reusing code.
	DECLARE_WND_CLASS_EX(TEXT("{DC2917D5-1288-4434-A28C-F16CFCE13C4B}"), CS_VREDRAW | CS_HREDRAW, (-1));

	void initialize_window(HWND parent)
	{
		WIN32_OP(Create(parent, 0, 0, 0, WS_EX_STATICEDGE) != NULL);
	}

	BEGIN_MSG_MAP(CMyElemWindowB)
	END_MSG_MAP()

	CMyElemWindowB(ui_element_config::ptr, ui_element_instance_callback_ptr p_callback);
	HWND get_wnd() { return *this; }
	void set_configuration(ui_element_config::ptr config) { m_config = config; }
	ui_element_config::ptr get_configuration() { return m_config; }
	static GUID g_get_guid() {
		// This is our GUID. Substitute with your own when reusing code.
		static const GUID guid_myelem = { 0xffffffff, 0x6cfb, 0x48d8, { 0xb6, 0xfe, 0x20, 0x0c, 0x47, 0x52, 0x2f, 0x2f } };
		return guid_myelem;
	}
	static GUID g_get_subclass() {
		return ui_element_subclass_containers; 
	}
	static void g_get_name(pfc::string_base & out) { out = "Sample UI Element"; }
	static ui_element_config::ptr g_get_default_configuration()
	{
		return ui_element_config::g_create_empty(g_get_guid());
	}
	static const char * g_get_description() { return "This is a sample UI Element."; }

	void notify(const GUID & p_what, t_size p_param1, const void * p_param2, t_size p_param2size) {};
private:
	ui_element_config::ptr m_config;
protected:
	const ui_element_instance_callback_ptr m_callback;
};

class ui_element_myinstance_callback : public ui_element_instance_callback
{
public:
	int FB2KAPI service_release() throw() { return 1; }
	int FB2KAPI service_add_ref() throw() { return 1; }

	void on_min_max_info_change() {};
	//! Deprecated, does nothing.
	void on_alt_pressed(bool p_state) {};
	//! Returns true on success, false when the color is undefined and defaults such as global windows settings should be used.
	bool query_color(const GUID & p_what, t_ui_color & p_out) { return false; }
	//! Tells the host that specified element wants to activate itself as a result of some kind of external user command (eg. menu command). Host should ensure that requesting child element's window is visible.\n
	//! @returns True when relevant child element window has been properly made visible and requesting code should proceed to SetFocus their window etc, false when denied.
	bool request_activation(service_ptr_t<class ui_element_instance> p_item) { return true; }

	//! Queries whether "edit mode" is enabled. Most of UI element editing functionality should be locked when it's not.
	bool is_edit_mode_enabled() { return true; }

	//! Tells the host that the user has requested the element to be replaced with another one.
	//! Note: this is generally used only when "edit mode" is enabled, but legal to call when it's not (eg. dummy element calls it regardless of settings when clicked).
	void request_replace(service_ptr_t<class ui_element_instance> p_item) {}

	//! Deprecated - use query_font_ex. Equivalent to query_font_ex(ui_font_default).
	t_ui_font query_font() { return query_font_ex(ui_font_default); }

	//! Retrieves an user-configurable font to use for specified kind of display. See ui_font_* constant for possible parameters.
	t_ui_font query_font_ex(const GUID & p_what) { return NULL; }

};


class ui_element_myimpl : public ui_element_impl<CMyElemWindowB> {};


