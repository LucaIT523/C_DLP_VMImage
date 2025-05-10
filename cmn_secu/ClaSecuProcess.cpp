#include "pch.h"
#include "ClaSecuProcess.h"
#include "ClaFileMap.h"
#include "ClaSecurityDescriptor.h"
#include "ClaPathMgr.h"
#include <cstdio>
#include <iostream>

#define LD_NAME_MAP_SECU		L"Global\\KenanMap-Secu"
#define LD_NAME_MUTEX_SECU		L"Global\\KenanMutex-Secu-%d"
ClaFileMap lv_filemapSecu;
void* lv_pSharedMem = NULL;

wchar_t g_wszDllPath[MAX_PATH];

void* OpenFileMapSecu() {
	if (lv_pSharedMem == NULL) {
		if (ClaFileMap::IsCreateMap(LD_NAME_MAP_SECU) == true) {
			lv_filemapSecu.openMap(LD_NAME_MAP_SECU, &lv_pSharedMem);
		}
		else {
			lv_filemapSecu.createMap(LD_NAME_MAP_SECU, sizeof(ST_SECU_MAP), &lv_pSharedMem);
		}
	}
	return lv_pSharedMem;
}

unsigned int ClaSecuProcess::RegisterSecuProcess(unsigned int p_nPID)
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = ClaSecurityDescriptor::GetSecurityDescriptor(MUTEX_ALL_ACCESS);

	CString strMutex; strMutex.Format(LD_NAME_MUTEX_SECU, p_nPID);

	CreateMutex(&sa, FALSE, strMutex);
	return 0;
}

BOOL ClaSecuProcess::IsSecuProcess(unsigned int p_nPID) {
	CString strMutex; strMutex.Format(LD_NAME_MUTEX_SECU, p_nPID);
	HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, strMutex);
	if (hMutex != NULL) {
		CloseHandle(hMutex);
		return TRUE;
	}
	if (GetLastError() == 5) {
		return TRUE;
	}
	return FALSE;
}

unsigned int ClaSecuProcess::RegisterClipboardFlag(BOOL p_bSecu)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return -1;

	pMap->m_bSecuClipboard = p_bSecu;
	return 0;
}

BOOL ClaSecuProcess::IsSecuClipboard(void) {
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return FALSE;
	return pMap->m_bSecuClipboard;
}

void ClaSecuProcess::InjectDll(HANDLE p_hProcess, LPCWSTR p_wszDllPath /*= NULL*/) {
	wchar_t wszDllPath[MAX_PATH]; memset(wszDllPath, 0, sizeof(wszDllPath));
	if (p_wszDllPath != NULL)
		wcscpy_s(wszDllPath, MAX_PATH, p_wszDllPath);
	else {
		wcscpy_s(wszDllPath, MAX_PATH, g_wszDllPath);
	}
	unsigned int cchDllPath = sizeof(wszDllPath) + 1;

	HMODULE hKernel32 = GetModuleHandleA("Kernel32");
	FARPROC lb = GetProcAddress(hKernel32, "LoadLibraryW");

	HANDLE hProcess = p_hProcess;

	void* pMem = VirtualAllocEx(hProcess, NULL, cchDllPath, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(hProcess, pMem, wszDllPath, cchDllPath, NULL);
	CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lb, pMem, 0, NULL);
}

unsigned int ClaSecuProcess::RegisterSecuPath(const wchar_t* p_wszPath)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();

	if (pMap == NULL) return -1;

	memset(pMap->m_wszSecu, 0, sizeof(pMap->m_wszSecu));
	wcscpy_s(pMap->m_wszSecu, p_wszPath);
}
BOOL ClaSecuProcess::IsSecuPathW(const wchar_t* p_wszPath)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();

	if (pMap == NULL) return FALSE;
	if (p_wszPath == NULL) return FALSE;
	if (_wcsnicmp(p_wszPath, pMap->m_wszSecu, wcslen(pMap->m_wszSecu)) == 0) {
		return TRUE;
	}
	return FALSE;
}

BOOL ClaSecuProcess::IsSecuPathA(const char* p_szPath)
{
	if (p_szPath == NULL) return FALSE;

	CString strW; strW.Format(L"%S", p_szPath);
	return IsSecuPathW(strW);
}

unsigned int ClaSecuProcess::ClearCryptoPath()
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();

	if (pMap == NULL) return -1;

	memset(pMap->m_wszCrypto, 0, sizeof(pMap->m_wszCrypto));
	pMap->m_nCrypo = 0;
}
unsigned int ClaSecuProcess::AddCryptoPath(const wchar_t* p_wszPath)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();

	if (pMap == NULL) return -1;

	wcscpy_s(pMap->m_wszCrypto[pMap->m_nCrypo], MAX_PATH, p_wszPath);
	pMap->m_nCrypo++;
}
BOOL ClaSecuProcess::IsCryptoPathW(const wchar_t* p_wszPath)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();

	if (pMap == NULL) return FALSE;
	if (p_wszPath == NULL) return FALSE;

	for (int i = 0; i < pMap->m_nCrypo; i++) {
		if (_wcsnicmp(p_wszPath, pMap->m_wszCrypto[i], wcslen(pMap->m_wszCrypto[i])) == 0) {
			return TRUE;
		}
	}
	return FALSE;
}
BOOL ClaSecuProcess::IsCryptoPathA(const char* p_szPath)
{
	if (p_szPath == NULL) return FALSE;

	CString strW; strW.Format(L"%S", p_szPath);
	return IsCryptoPathW(strW);
}

unsigned int ClaSecuProcess::ClearExceptPath()
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();

	if (pMap == NULL) return -1;

	memset(pMap->m_wszExcept, 0, sizeof(pMap->m_wszExcept));
	pMap->m_nExcept = 0;
}
unsigned int ClaSecuProcess::AddExceptPath(const wchar_t* p_wszPath)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();

	if (pMap == NULL) return -1;

	wcscpy_s(pMap->m_wszExcept[pMap->m_nExcept], MAX_PATH, p_wszPath);
	pMap->m_nExcept++;
}
BOOL ClaSecuProcess::IsExceptPathW(const wchar_t* p_wszPath)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();

	if (pMap == NULL) return FALSE;
	if (p_wszPath == NULL) return FALSE;

	for (int i = 0; i < pMap->m_nExcept; i++) {
		if (_wcsnicmp(p_wszPath, pMap->m_wszExcept[i], wcslen(pMap->m_wszExcept[i])) == 0) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ClaSecuProcess::IsExceptPathA(const char* p_szPath)
{
	if (p_szPath == NULL) return FALSE;

	CString strW; strW.Format(L"%S", p_szPath);
	return IsExceptPathW(strW);
}

unsigned int ClaSecuProcess::RegisterTempPath(const wchar_t* p_wszPath)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return -1;
	memset(pMap->m_wszTemp, 0, sizeof(pMap->m_wszTemp));
	wcscpy_s(pMap->m_wszTemp, p_wszPath);
	return 0;
}
unsigned int ClaSecuProcess::GetTempFilePathW(wchar_t* p_wszPath)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();

	if (pMap == NULL) return -1;

	char tempFilename[L_tmpnam];
	tmpnam_s(tempFilename, L_tmpnam);
	char* pPos = strrchr(tempFilename, '\\');
	
	swprintf_s(p_wszPath, MAX_PATH, L"%s\\%S", pMap->m_wszTemp, &pPos[1]);
	return 0;
}

unsigned int ClaSecuProcess::GetTempFilePathA(char* p_szPath)
{
	wchar_t wszRet[MAX_PATH]; memset(wszRet, 0, sizeof(wszRet));
	if (GetTempFilePathW(wszRet) != 0) return -1;

	sprintf_s(p_szPath, MAX_PATH, "%S", wszRet);
	return 0;
}

