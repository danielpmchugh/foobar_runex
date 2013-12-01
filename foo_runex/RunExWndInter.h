#ifndef RUNEXWND_INTER_H
#define RUNEXWND_INTER_H

class CRunExWnd;

class CRunExWndInter 
{
public:
	static CRunExWnd * runExWnd;

	static void ShowWindow();
	static void HideWindow();
	static void SaveConifg();

};

#endif

