#ifndef HOOKIDLE_H
#define HOOKIDLE_H

#include "..\stdafx.h"
#include "smartptr.h"

namespace Utils{

	//istal a ForegroundIdleProc Hook and chain the instances.
	// when the hook is called, the OnIdle finctions are called in turn

	// handle always on the heap, using strong poiters

	class CHookIdle;
	typedef Safe::PtrStrong<CHookIdle> PHookIdle;

	class CHookIdle: public Safe::ObjStrong
	{
	private:
		LPVOID _token;
	protected:
		LRESULT Default();
		virtual LRESULT OnIdle(); //override this function. call Default as appropriate
	public:
		CHookIdle();
		virtual ~CHookIdle(void);
		static LRESULT Call(LPVOID token);
		bool Clear();
	};


}

#endif