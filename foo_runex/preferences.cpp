#include "stdafx.h"
#include "resource.h"
#include "window_helper.h"
#include "RunExWnd.h"
#include "preferences.h"

// Default setting for Preferences screen
const bool CMyPreferences::default_cfg_IsBtnText        = TRUE;
const std::string CMyPreferences::default_cfg_BtnText   = "RunEx";
const std::string CMyPreferences::default_cfg_ImageText = "";
const std::string CMyPreferences::default_cfg_Action    = "";


// GUID uniquely identify configuration settings- If code is reused these would need to be regenerated,
// I used http://www.guidgenerator.com/online-guid-generator.aspx to generate these IDs
// GUID- Identify configuration settings
const GUID CMyPreferences::guid_cfg_BtnText		= { 0xaed7b184, 0x49a5, 0x4b25, { 0x86, 0x77, 0xe7, 0x1b, 0x2b, 0x04, 0xba, 0x7e } };
const GUID CMyPreferences::guid_cfg_IsBtnText	= { 0x5a596151, 0x5888, 0x4698, { 0x95, 0xfb, 0x45, 0x65, 0x9c, 0xd7, 0x95, 0x66 } };
const GUID CMyPreferences::guid_cfg_ImageText	= { 0x3786f395, 0x1121, 0x489d, { 0x9d, 0x1f, 0xea, 0xc2, 0xaf, 0xc5, 0x60, 0x3e } };
const GUID CMyPreferences::guid_cfg_Action		= { 0x87754312, 0x8df3, 0x4674, { 0x84, 0x36, 0xf1, 0x81, 0x37, 0xc7, 0x34, 0x15 } };

// This GUID identifies our Advanced Preferences branch 
const GUID CMyPreferences::guid_advconfig_runex = { 0x14ac1645, 0x138c, 0x436b, { 0xaa, 0x67, 0x6e, 0xf3, 0xde, 0xb0, 0x51, 0x6e } };

cfg_bool CMyPreferences::cfg_IsBtnText	(guid_cfg_IsBtnText, default_cfg_IsBtnText );
cfg_string CMyPreferences::cfg_BtnText	(guid_cfg_BtnText, default_cfg_BtnText.c_str());
cfg_string CMyPreferences::cfg_ImageText (guid_cfg_ImageText, default_cfg_ImageText.c_str());
cfg_string CMyPreferences::cfg_Action	(guid_cfg_Action, default_cfg_Action.c_str());


advconfig_branch_factory  CMyPreferences::g_advconfigrunex  ("RunEx",			   guid_advconfig_runex, advconfig_branch::guid_branch_tools, 0);
advconfig_string_factory  CMyPreferences::cfg_runexBtnText  ("Button Text",	   guid_cfg_BtnText, guid_advconfig_runex, 0, default_cfg_BtnText.c_str());
advconfig_string_factory  CMyPreferences::cfg_runexImageText("Image Text",	   guid_cfg_ImageText, guid_advconfig_runex, 0, default_cfg_ImageText.c_str());
advconfig_string_factory  CMyPreferences::cfg_runexAction   ("Action",		   guid_cfg_Action, guid_advconfig_runex, 0, default_cfg_Action.c_str());
advconfig_integer_factory CMyPreferences::cfg_runexIsBtnText("Is Text Selected", guid_cfg_IsBtnText, guid_advconfig_runex, 0, default_cfg_IsBtnText, 0 /*minimum value*/, 1 /*maximum value*/);

LRESULT CMyPreferences::OnInitDialog(UINT,WPARAM,LPARAM,BOOL&)
{		
	std::wstring wImageText;
	std::wstring wBtnText;
	std::wstring wAction;

	CMyPreferences::StringConfigtoWString (wImageText, cfg_ImageText);
	CMyPreferences::StringConfigtoWString (wBtnText,   cfg_BtnText );	
	CMyPreferences::StringConfigtoWString (wAction,	   cfg_Action);
			
	// Convert to unicode
	SetDlgItemText(IDC_BTNTEXT, wBtnText.c_str());
	SetDlgItemText(IDC_IMAGE, wImageText.c_str());
	SetDlgItemText(IDC_ACTION, wAction.c_str());	

	if (cfg_IsBtnText == TRUE)
	{
		CheckDlgButton (IDC_RADIO1, BST_CHECKED);
		CheckDlgButton (IDC_RADIO2, BST_UNCHECKED);	
	}
	else
	{
		CheckDlgButton (IDC_RADIO1, BST_UNCHECKED);	
		CheckDlgButton (IDC_RADIO2, BST_CHECKED);		
	}

	return 0;
}

LRESULT CMyPreferences::OnEditChange(WPARAM,LPARAM,HWND, BOOL&)
{	
	OnChanged();
	return 0;
}

t_uint32 CMyPreferences::get_state() {
	t_uint32 state = preferences_state::resettable;
	if (HasChanged()) state |= preferences_state::changed;
	return state;
}

void CMyPreferences::reset() {
	std::wstring temp; 
	StringToWString (temp, default_cfg_BtnText.c_str());
	SetDlgItemText(IDC_BTNTEXT, temp.c_str());

	StringToWString (temp, default_cfg_ImageText.c_str());
	SetDlgItemText(IDC_IMAGE, temp.c_str());

	StringToWString (temp, default_cfg_Action.c_str());
	SetDlgItemText(IDC_ACTION, temp.c_str());	
	
	if (default_cfg_IsBtnText == TRUE)
	{
		CheckDlgButton (IDC_RADIO1, BST_CHECKED);
		CheckDlgButton (IDC_RADIO2, BST_UNCHECKED);	
	}
	else
	{
		CheckDlgButton (IDC_RADIO1, BST_UNCHECKED);	
		CheckDlgButton (IDC_RADIO2, BST_CHECKED);		
	}

	OnChanged();
}

int CMyPreferences::StringToWString(std::wstring &ws, const std::string &s)
{
    std::wstring wsTmp(s.begin(), s.end());
    ws = wsTmp;
    return 0;
}


int CMyPreferences::WStringToString(std::string &s, const std::wstring &ws)
{
    std::string sTmp(ws.begin(), ws.end());
    s = sTmp;
    return 0;
}


int CMyPreferences::StringConfigtoString (std::string &text, const cfg_string &cfg)
{
	pfc::string8 format;
	format = cfg;
	text = format;			
	return 0;
}





int CMyPreferences::StringConfigtoWString (std::wstring &wText, const cfg_string &cfg)
{
	pfc::string8 format = cfg;	
	std::string sText = format;		
	StringToWString (wText, sText);
	return 0;
}

int CMyPreferences::DlgToString (std::string &text, int id)
{
	TCHAR tText[1024];
	LPTSTR str=tText;
	GetDlgItemText(id, str, 255);	
	std::wstring w = str;	
	std::string sTmp(w.begin(), w.end());
	text = sTmp;

	return 0;
}


void CMyPreferences::apply() {
	
	std::string text;
	DlgToString (text, IDC_BTNTEXT);
	cfg_BtnText = text.c_str();

	DlgToString (text, IDC_IMAGE);
	cfg_ImageText = text.c_str();

	DlgToString (text, IDC_ACTION);
	cfg_Action = text.c_str();

	cfg_IsBtnText = (IsDlgButtonChecked(IDC_RADIO1) == BST_CHECKED);

	CWnd * cWnd = CWnd::FromHandle(core_api::get_main_window());
	HWND hwndOwner = cWnd->GetSafeHwnd();		
	CWnd * tWnd = cWnd->FindWindowExW(core_api::get_main_window(),NULL,L"RunExWnd",NULL);
	SendMessage(CRunExWnd::hWnd, WM_UPDATE_CTRL, 0, 0);
	OnChanged(); //our dialog content has not changed but the flags have - our currently shown values now match the settings so the apply button can be disabled
}

bool CMyPreferences::CompareCfgToDlg (int IDC, const cfg_string& cfg )
{
	std::string dlgText;
	DlgToString (dlgText, IDC);	
	std::string cfgText;
	CMyPreferences::StringConfigtoString (cfgText, cfg);

	return (cfgText.compare(dlgText) != 0);		
}

bool CMyPreferences::HasChanged() {
	//returns whether our dialog content is different from the current configuration (whether the apply button should be enabled or not)
	bool bHasChanged = false;

	if (bHasChanged == false)
		bHasChanged = CompareCfgToDlg(IDC_BTNTEXT,cfg_BtnText);
	if (bHasChanged == false)
		bHasChanged = CompareCfgToDlg(IDC_IMAGE,cfg_ImageText);
	if (bHasChanged == false)
		bHasChanged = CompareCfgToDlg(IDC_ACTION,cfg_Action);
	if (bHasChanged == false)
		bHasChanged = (IsDlgButtonChecked(IDC_RADIO1) == BST_CHECKED) != (cfg_IsBtnText.get_value());			
	return  bHasChanged;
}
void CMyPreferences::OnChanged() {
	//tell the host that our state has changed to enable/disable the apply button appropriately.
	m_callback->on_state_changed();
}

class preferences_page_myimpl : public preferences_page_impl<CMyPreferences> {
	// preferences_page_impl<> helper deals with instantiation of our dialog; inherits from preferences_page_v3.
public:
	const char * get_name() {return "RunEx";}
	GUID get_guid() {
		// This is our GUID. Replace with your own when reusing the code.
		static const GUID guid = { 0x09537f9d, 0xc5e6, 0x4a27, { 0xaf, 0x79, 0x39, 0x17, 0x05, 0xc4, 0x68, 0x28 } };
		
		return guid;
	}
	GUID get_parent_guid() {return guid_tools;}
};

static preferences_page_factory_t<preferences_page_myimpl> g_preferences_page_myimpl_factory;



