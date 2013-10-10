#include "StdAfx.h"
#include "strace.h"

using namespace Mfc;


int STrace::_nLevel = 0;

STrace::STrace(void)
{
	_nLevel++;
}

STrace::~STrace(void)
{
	_nLevel--;
}

void STrace::Trace(const type_info& t, LPVOID pAddress, CString text)
{
	CString fmt('.',_nLevel);
	fmt += " %p: %s: %s \n";
//	TRACE((LPCTSTR)fmt, pAddress, t.name(), (LPCTSTR)text);
}
