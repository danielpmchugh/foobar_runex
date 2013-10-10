#include "stdafx.h"

class CMyPreferences : public CDialogImpl<CMyPreferences>, public preferences_page_instance {
public:
	//Constructor - invoked by preferences_page_impl helpers - don't do Create() in here, preferences_page_impl does this for us
	CMyPreferences(preferences_page_callback::ptr callback) : m_callback(callback) {}

	//dialog resource ID
	enum {IDD = IDD_RUNEXPREFERENCES};
	// preferences_page_instance methods (not all of them - get_wnd() is supplied by preferences_page_impl helpers)
	t_uint32 get_state();
	void apply();
	void reset();

	//WTL message map
	BEGIN_MSG_MAP(CMyPreferences)
		MESSAGE_HANDLER(WM_INITDIALOG,OnInitDialog)				
		COMMAND_HANDLER(IDC_BTNTEXT, EN_CHANGE, OnEditChange)
		COMMAND_HANDLER(IDC_IMAGE, EN_CHANGE, OnEditChange)
		COMMAND_HANDLER(IDC_ACTION, EN_CHANGE, OnEditChange)
		COMMAND_HANDLER(IDC_RADIO1, BN_CLICKED, OnEditChange)
		COMMAND_HANDLER(IDC_RADIO2, BN_CLICKED, OnEditChange)
	END_MSG_MAP()
private:

	LRESULT OnInitDialog(UINT,WPARAM,LPARAM,BOOL&);
    LRESULT OnEditChange(WPARAM,LPARAM,HWND, BOOL&);
	bool HasChanged();
	void OnChanged();

public:
	//Helper functins
	int DlgToString (std::string &wText, int id);
	static int StringToWString(std::wstring &ws, const std::string &s);
	static int StringConfigtoWString (std::wstring &wText, const cfg_string &cfg);
	static int StringConfigtoString  (std::string &text, const cfg_string &cfg);	
	static int WStringToString(std::string &s, const std::wstring &ws);
	bool CompareCfgToDlg (int IDC, const cfg_string& cfg );
public:
	const preferences_page_callback::ptr m_callback;
	LRESULT OnEnChangeRunEx(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedRadio1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedRadio2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

public:
	// Default setting for Preferences screen
	static const bool default_cfg_IsBtnText;
	static const std::string default_cfg_BtnText;
	static const std::string default_cfg_ImageText;
	static const std::string default_cfg_Action;
	
	// GUID uniquely identify configuration settings- If code is reused these would need to be regenerated,
	// I used http://www.guidgenerator.com/online-guid-generator.aspx to generate these IDs
	// GUID- Identify configuration settings
	static const GUID guid_cfg_BtnText;
	static const GUID guid_cfg_IsBtnText;
	static const GUID guid_cfg_ImageText;
	static const GUID guid_cfg_Action;
	// This GUID identifies our Advanced Preferences branch 
	static const GUID guid_advconfig_runex;
	
	static cfg_bool cfg_IsBtnText;
	static cfg_string cfg_BtnText;
	static cfg_string cfg_ImageText;
	static cfg_string cfg_Action;
	
	static advconfig_branch_factory  g_advconfigrunex;
	static advconfig_string_factory  cfg_runexBtnText;
	static advconfig_string_factory  cfg_runexImageText;
	static advconfig_string_factory  cfg_runexAction;
	static advconfig_integer_factory cfg_runexIsBtnText;
	
};
