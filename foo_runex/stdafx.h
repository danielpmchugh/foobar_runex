#pragma once
#define _WINSOCKAPI_


#define WM_UPDATE_CTRL			WM_USER + 100
#define WM_TOOLBAR_SHOWHIDE		WM_USER + 101
#define WM_CONTEXT_MENU_FB		WM_USER + 102

#include <winsock2.h>

#include "../SDK/foobar2000.h"

#include "resource.h"
#undef	_WINDOWS_

#include <afxwin.h>
#include <afxdisp.h>
#include <AFXOLEDB.H>
#define USE_MFC6_WITH_ATL7
#include <atlbase.h>
#include <atlwin.h>

#include "../helpers/win32_misc.h"
#include "../ATLHelpers/misc.h"