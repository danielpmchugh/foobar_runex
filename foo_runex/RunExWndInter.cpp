#include "stdafx.h"
#include "RunExWndInter.h"
#include "RunExWnd.h"

CRunExWnd * CRunExWndInter::runExWnd = NULL;

void CRunExWndInter::ShowWindow()
{
	if (runExWnd == NULL)
	{	
		runExWnd = new CRunExWnd();
		runExWnd->Create(core_api::get_main_window());
		runExWnd->ShowWindow();
	}

}

void CRunExWndInter::HideWindow()
{
	runExWnd->HideWindow();

}

void CRunExWndInter::SaveConifg()
{
	runExWnd->SaveConfig();
}
