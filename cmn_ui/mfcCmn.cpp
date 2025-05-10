#include "pch.h"
#include "mfcCmn.h"

CString getSize(unsigned long long l)
{
	int d, s;
	CString u;

	if (l > (1 << 30)) {
		d = l >> 30;
		s = ((l * 100) / (1 << 30)) % 100;
		u = L"GB";
	}
	else if (l > (1 << 20)) {
		d = l >> 20;
		s = ((l * 100) / (1 << 20)) % 100;
		u = L"MB";
	}
	else if (l > (1 << 10)) {
		d = l >> 10;
		s = ((l * 100) / (1 << 10)) % 100;
		u = L"KB";
	}
	else {
		d = l; s = 0;
	}

	CString ret; ret.Format(L"%d.%d", d, s);
	ret.TrimRight(L"0");
	ret.TrimRight(L".");
	ret += L" " + u;
	return ret;
}

unsigned int MsgBox(const wchar_t* p_wszFmt, ...)
{
	CString strMsg;
	va_list vl;
	va_start(vl, p_wszFmt);

	strMsg.FormatV(p_wszFmt, vl);
	return AfxMessageBox(strMsg);
}

void LogPrint(const wchar_t* p_wszFmt, ...)
{
	CString strMsg;
	va_list vl;
	va_start(vl, p_wszFmt);

	strMsg.FormatV(p_wszFmt, vl);
	OutputDebugStringW(strMsg);
}