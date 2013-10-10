#include "stdafx.h"
#include "window_helper.h"


PFC_NORETURN PFC_NOINLINE void WIN32_OP_FAIL() {
	const DWORD code = GetLastError();
	PFC_ASSERT( code != NO_ERROR );
	pfc::string_fixed_t<32> debugMsg; debugMsg << "Win32 error #" << (t_uint32)code;
	TRACK_CODE( debugMsg, throw exception_win32(code) );
}


#ifdef _DEBUG
void WIN32_OP_D_FAIL(const wchar_t * _Message, const wchar_t *_File, unsigned _Line) {
	const DWORD code = GetLastError();
	pfc::array_t<wchar_t> msgFormatted; msgFormatted.set_size(pfc::strlen_t(_Message) + 64);
	wsprintfW(msgFormatted.get_ptr(), L"%s (code: %u)", _Message, code);
	if (IsDebuggerPresent()) {
		OutputDebugString(TEXT("WIN32_OP_D() failure:\n"));
		OutputDebugString(msgFormatted.get_ptr());
		OutputDebugString(TEXT("\n"));
		pfc::crash();
	}
	assert(msgFormatted.get_ptr());
}
#endif