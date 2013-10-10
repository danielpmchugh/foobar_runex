#pragma once
#include "..\stdafx.h"
namespace Mfc{

	//class STrace: keeps track of indented tracing

	class STrace
	{
	private:
		static int _nLevel;
	public:
		STrace(void);
		~STrace(void);
		void Trace(const type_info& type, LPVOID pAddress, CString text);
	};

}
