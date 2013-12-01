#pragma once

#include "Smartptr.h"
#undef	_WINDOWS_
#include <afxwin.h>
namespace Utils{

	//provide a way to sublass a window.
	//override the virtual OnWndProcSub to process messages
	
	//NOTE: the object must be always on heap
	// to keep track of message recursion. 

	//ex. if the sublaccer is associateed to a CFrameWnd or CView,
	// WM_NCDESTROY will cause the deletion of the CFrameWnd or CView.
	//	howver, there are other message on the stack (for ex. WM_SYSCOMMAND) that still need to be processed 

	/// alwaus allocate on the heap and manage with strong pointers

	class CWndSubclasser;
	typedef Safe::PtrStrong<CWndSubclasser> PWndSubclasser;

	class CWndSubclasser: public Safe::ObjStrong
	{
	private:
		LPVOID _token;
		bool _bCleared;
	protected:
		HWND _hWnd; //referred window
	public:
		CWndSubclasser();
		virtual ~CWndSubclasser(void);
		bool Subclass(HWND hWnd);		
		bool Clear();
		static LRESULT Call(LPVOID token);
		LRESULT Default();
		LRESULT Default(WPARAM wParam, LPARAM lParam);
		CWnd* GetWnd() {return CWnd::FromHandle(_hWnd);}
	protected:
		virtual LRESULT WndProcSub(UINT uMsg, WPARAM wParam, LPARAM lParam);
	};


}


