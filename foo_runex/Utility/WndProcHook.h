#pragma once

#include "Smartptr.h"

namespace Utils{

	// this calss installs a WH_CALLWNDPROC hook, chaining the instances,
	// and calling the virtual OnHook. You can derive and specialize OnHook as needed

	// always allocate on heap and manage with strong poiters

	class CWndProcHook;
	typedef Safe::PtrStrong<CWndProcHook> PWndProcHook;

	class CWndProcHook: public Safe::ObjStrong
	{
	private:
		LPVOID _token;
	public:
		static LRESULT Call(LPVOID token);
		LRESULT Default();
		bool Clear();
		CWndProcHook();
		int CWndProcHook::CreateHook();
		virtual ~CWndProcHook(void);
	protected:
		virtual LRESULT OnHook(CWPSTRUCT& cwps) = 0;
	};

    
}

