#include "pch.h"
#include "ClaPathMgr.h"

CString ClaPathMgr::GetFN(const wchar_t* p_wszPath /*= NULL*/, BOOL p_bWithExt /*= TRUE*/)
{
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
	if (p_wszPath == NULL) {
		GetModuleFileName(NULL, wszPath, MAX_PATH);
	}
	else {
		wcscpy_s(wszPath, MAX_PATH, p_wszPath);
	}
	wchar_t* pPos = wcsrchr(wszPath, L'\\');

	if (pPos == NULL) return L"";

	if (!p_bWithExt) {
		wchar_t* pExt = wcsrchr(wszPath, L'.');
		if (pExt != NULL) pExt[0] = 0x0;
	}

	return &pPos[1];
}

CString ClaPathMgr::GetDP(const wchar_t* p_wszPath /*= NULL*/)
{
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
	if (p_wszPath == NULL) {
		GetModuleFileName(NULL, wszPath, MAX_PATH);
	}
	else {
		wcscpy_s(wszPath, MAX_PATH, p_wszPath);
	}
	wchar_t* pPos = wcsrchr(wszPath, L'\\');
	if (pPos != NULL) pPos[0] = 0;
	return wszPath;
}

CString ClaPathMgr::GetUserAppData(const wchar_t* p_wszAdditional)
{
	wchar_t appDataPath[MAX_PATH]; memset(appDataPath, 0, sizeof(appDataPath));
	HRESULT result = SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appDataPath);
	CString strRet;
	if (p_wszAdditional)
		strRet.Format(L"%s\\%s", appDataPath, p_wszAdditional);
	else
		strRet.Format(L"%s", appDataPath);
	return strRet;
}

CString ClaPathMgr::GetUserPath(const wchar_t* p_wszAdditional)
{
	wchar_t appDataPath[MAX_PATH]; memset(appDataPath, 0, sizeof(appDataPath));
	SHGetSpecialFolderPath(NULL, appDataPath, CSIDL_PROFILE, TRUE);

	CString strRet;
	if (p_wszAdditional)
		strRet.Format(L"%s\\%s", appDataPath, p_wszAdditional);
	else
		strRet.Format(L"%s", appDataPath);
	return strRet;
}
CString ClaPathMgr::GetUserRoaming(const wchar_t* p_wszAdditional /*= NULL*/)
{
	wchar_t appDataPath[MAX_PATH]; memset(appDataPath, 0, sizeof(appDataPath));
	HRESULT result = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appDataPath);
	CString strRet;
	if (p_wszAdditional)
		strRet.Format(L"%s\\%s", appDataPath, p_wszAdditional);
	else
		strRet.Format(L"%s", appDataPath);
	return strRet;
}

int ClaPathMgr::CmpProcName(const wchar_t* p_wszPath, const wchar_t* p_wszExe)
{
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
	wcscpy_s(wszPath, MAX_PATH, GetFN(p_wszPath));
	_wcsupr_s(wszPath, MAX_PATH);
	wchar_t wszExe[64]; memset(wszExe, 0, sizeof(wszExe));
	wcscpy_s(wszExe, 64, p_wszExe);
	_wcsupr_s(wszExe);

	return wcscmp(wszPath, wszExe);
}

int _executeEx(const wchar_t* p_szEXE, const wchar_t* p_pszCommandParam)
{
	SHELLEXECUTEINFO ShExecInfo;
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = L"open";
	ShExecInfo.lpFile = p_szEXE;
	ShExecInfo.lpParameters = p_pszCommandParam; //  L"/C apktool.bat -f d D:\\work\\_FCM\\test_org.apk -o D:\\work\\_FCM\\aaa";
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE; // SW_NORMAL

	if (ShellExecuteEx(&ShExecInfo)) {
		// Wait for the process to exit
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	}
	return 1;
}

void ClaPathMgr::DeleteDirectory(const wchar_t* p_wszPath)
{
	CString strCmd;
	strCmd.Format(L"/C rd /s /q \"%s\"", p_wszPath);
	_executeEx(L"cmd.exe", strCmd);
}
BOOL ClaPathMgr::UnzipFile(const wchar_t* p_7zipExePath, const wchar_t* p_command) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags |= STARTF_USESHOWWINDOW;;
	si.wShowWindow = SW_HIDE;

	wchar_t command[512];
	swprintf_s(command, 512, L"\%s\\7z.exe %s", p_7zipExePath, p_command);

	if (CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) == FALSE) {
		return FALSE;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	return TRUE;
}

long long ClaPathMgr::GetFileSize(const wchar_t* p_wszPath) {
	struct _stat fileInfo;

	// Get file information
	if (_wstat(p_wszPath, &fileInfo) != 0) {
		return -1; // Return -1 to indicate an error
	}

	// Return the size of the file
	return fileInfo.st_size;
}