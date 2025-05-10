#include "pch.h"
#include "ClaSecuPolicy.h"
#include "ClaFileMap.h"
#include "ClaSecurityDescriptor.h"
#include "ClaPathMgr.h"
#include <cstdio>
#include <iostream>

#define LD_NAME_MAP_SECU		L"Global\\KenanMap-Secu"
#define LD_NAME_MUTEX_SECU		L"Global\\KenanMutex-Secu-%d"
ClaFileMap lv_filemapSecu;
void* lv_pSharedMem = NULL;
ST_SECU_PATH_MAP lv_SecuPathMap = { 0, };
wchar_t g_wszDllPath[MAX_PATH];

bool ntpath_cmp(const wchar_t* p_wszDir, const wchar_t* p_wszPath) {
	if (p_wszPath == NULL) return false;

	if (_wcsnicmp(p_wszPath, p_wszDir, wcslen(p_wszDir)) == 0) {
		return true;
	}
	if (wcslen(p_wszPath) > 4 && wcsncmp(p_wszPath, L"\\??\\", 4) == 0
	&& _wcsnicmp(&p_wszPath[4], p_wszDir, wcslen(p_wszDir)) == 0) {
		return true;
	}
	return false;
}

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

unsigned int ClaSecuPolicy::RegisterSecuProcess(unsigned int p_nPID)
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = ClaSecurityDescriptor::GetSecurityDescriptor(MUTEX_ALL_ACCESS);

	CString strMutex; strMutex.Format(LD_NAME_MUTEX_SECU, p_nPID);

	CreateMutex(&sa, FALSE, strMutex);

	delete sa.lpSecurityDescriptor;
	return 0;
}

BOOL ClaSecuPolicy::IsSecuProcess(unsigned int p_nPID) {
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

unsigned int ClaSecuPolicy::RegisterClipboardFlag(BOOL p_bSecu)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return -1;

	pMap->m_bSecuClipboard = p_bSecu;
	return 0;
}

BOOL ClaSecuPolicy::IsSecuClipboard(void) {
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return FALSE;
	return pMap->m_bSecuClipboard;
}

void ClaSecuPolicy::InjectDll(HANDLE p_hProcess, LPCWSTR p_wszDllPath /*= NULL*/) {
	wchar_t wszDllPath[MAX_PATH]; memset(wszDllPath, 0, sizeof(wszDllPath));
	if (p_wszDllPath != NULL)
		wcscpy_s(wszDllPath, MAX_PATH, p_wszDllPath);
	else {
		wcscpy_s(wszDllPath, MAX_PATH, g_wszDllPath);
	}
	unsigned int cchDllPath = sizeof(wszDllPath) + 1;

	HMODULE hKernel32 = GetModuleHandleA("KernelBase.dll");
	FARPROC lb = GetProcAddress(hKernel32, "LoadLibraryW");

	HANDLE hProcess = p_hProcess;

	void* pMem = VirtualAllocEx(hProcess, NULL, cchDllPath, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(hProcess, pMem, wszDllPath, cchDllPath, NULL);
	HANDLE h = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lb, pMem, 0, NULL);
	WaitForSingleObject(h, 1000);
	VirtualFreeEx(hProcess, pMem, 0, MEM_RELEASE);
}
void ClaSecuPolicy::InjectDllAsUser(int p_nPID, LPCWSTR p_wszDllPath /*= NULL*/) {

}

unsigned int ClaSecuPolicy::RegisterSecuPath(const wchar_t* p_wszPath)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();

	if (pMap == NULL) return -1;

	memset(pMap->m_wszSecu, 0, sizeof(pMap->m_wszSecu));
	wcscpy_s(pMap->m_wszSecu, p_wszPath);
	return 0;
}
BOOL ClaSecuPolicy::IsSecuPathW(const wchar_t* p_wszPath)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();

	if (pMap == NULL) return FALSE;
	if (p_wszPath == NULL) return FALSE;

	return ntpath_cmp(pMap->m_wszSecu, p_wszPath);
}

BOOL ClaSecuPolicy::IsSecuPathA(const char* p_szPath)
{
	if (p_szPath == NULL) return FALSE;

	CString strW; strW.Format(L"%S", p_szPath);
	return IsSecuPathW(strW);
}

unsigned int ClaSecuPolicy::ClearCryptoPath()
{
	memset(lv_SecuPathMap.m_wszCrypto, 0, sizeof(lv_SecuPathMap.m_wszCrypto));
	lv_SecuPathMap.m_nCrypo = 0;
	return 0;
}
unsigned int ClaSecuPolicy::AddCryptoPath(const wchar_t* p_wszPath)
{
	wcscpy_s(lv_SecuPathMap.m_wszCrypto[lv_SecuPathMap.m_nCrypo], MAX_PATH, p_wszPath);
	lv_SecuPathMap.m_nCrypo++;
	return 0;
}
BOOL ClaSecuPolicy::IsCryptoPathW(const wchar_t* p_wszPath)
{
	if (p_wszPath == NULL) return FALSE;

	for (int i = 0; i < lv_SecuPathMap.m_nCrypo; i++) {
		if (ntpath_cmp(lv_SecuPathMap.m_wszCrypto[i], p_wszPath)) {
			return TRUE;
		}
	}
	return FALSE;
}
BOOL ClaSecuPolicy::IsCryptoPathA(const char* p_szPath)
{
	if (p_szPath == NULL) return FALSE;

	CString strW; strW.Format(L"%S", p_szPath);
	return IsCryptoPathW(strW);
}

unsigned int ClaSecuPolicy::ClearExceptPath()
{
	memset(lv_SecuPathMap.m_wszExcept, 0, sizeof(lv_SecuPathMap.m_wszExcept));
	lv_SecuPathMap.m_nExcept = 0;
	return 0;
}
unsigned int ClaSecuPolicy::AddExceptPath(const wchar_t* p_wszPath)
{
	wcscpy_s(lv_SecuPathMap.m_wszExcept[lv_SecuPathMap.m_nExcept], MAX_PATH, p_wszPath);
	lv_SecuPathMap.m_nExcept++;
	return 0;
}
BOOL ClaSecuPolicy::IsExceptPathW(const wchar_t* p_wszPath)
{
	if (p_wszPath == NULL) return FALSE;
	if (_wcsnicmp(p_wszPath, L"\\??\\pipe", wcslen(L"\\??\\pipe")) == 0) return TRUE;

	for (int i = 0; i < lv_SecuPathMap.m_nExcept; i++) {
		if (ntpath_cmp(lv_SecuPathMap.m_wszExcept[i], p_wszPath)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ClaSecuPolicy::IsExceptPathA(const char* p_szPath)
{
	if (p_szPath == NULL) return FALSE;

	CString strW; strW.Format(L"%S", p_szPath);
	return IsExceptPathW(strW);
}

unsigned int ClaSecuPolicy::RegisterTempPath(const wchar_t* p_wszPath)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return -1;
	memset(pMap->m_wszTemp, 0, sizeof(pMap->m_wszTemp));
	wcscpy_s(pMap->m_wszTemp, p_wszPath);
	return 0;
}

BOOL ClaSecuPolicy::IsTempPathW(const wchar_t* p_wszPath)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();

	if (pMap == NULL) return FALSE;
	if (p_wszPath == NULL) return FALSE;
	if (ntpath_cmp(pMap->m_wszTemp, p_wszPath)) {
		return TRUE;
	}
	return FALSE;
}

unsigned int ClaSecuPolicy::GenerateTempFilePathW(wchar_t* p_wszPath)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();

	if (pMap == NULL) return -1;

	char tempFilename[L_tmpnam];
	tmpnam_s(tempFilename, L_tmpnam);
	char* pPos = strrchr(tempFilename, '\\');
	
	swprintf_s(p_wszPath, MAX_PATH, L"\\??\\%s\\%S", pMap->m_wszTemp, &pPos[1]);
	return 0;
}

unsigned int ClaSecuPolicy::GenerateTempFilePathA(char* p_szPath)
{
	wchar_t wszRet[MAX_PATH]; memset(wszRet, 0, sizeof(wszRet));
	if (GenerateTempFilePathW(wszRet) != 0) return -1;

	sprintf_s(p_szPath, MAX_PATH, "%S", wszRet);
	return 0;
}

unsigned int ClaSecuPolicy::SetStartFlag(BOOL p_bFlag)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return -1;

	pMap->m_bStartFlag = p_bFlag;

	return 0;
}
BOOL ClaSecuPolicy::GetStartFlag()
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return FALSE;
	return pMap->m_bStartFlag == TRUE;
}

unsigned int ClaSecuPolicy::SetDlpFile(BOOL p_bEnable)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return -1;

	pMap->m_bDlpFile = p_bEnable;

	return 0;
}
unsigned int ClaSecuPolicy::SetDlpClipboard(BOOL p_bEnable)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return -1;

	pMap->m_bDlpClipboard = p_bEnable;

	return 0;
}
unsigned int ClaSecuPolicy::SetDlpCapture(BOOL p_bEnable)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return -1;

	pMap->m_bDlpCapture = p_bEnable;

	return 0;
}
unsigned int ClaSecuPolicy::SetDlpPrint(BOOL p_bEnablePdf, BOOL p_bEnableDev)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return -1;

	pMap->m_bDlpPrintPdf = p_bEnablePdf;
	pMap->m_bDlpPrintDev = p_bEnableDev;

	return 0;
}
unsigned int ClaSecuPolicy::SetDlpGlobalFlag(BOOL p_bEnable)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return -1;

	pMap->m_bDlpGlobal = p_bEnable;

	return 0;
}
unsigned int ClaSecuPolicy::SetDlpCryptoKey(unsigned char* p_pKey)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return -1;

	memcpy(pMap->m_pCryptoKey, p_pKey, 32);

	return 0;
}

BOOL ClaSecuPolicy::GetDlpFile()
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return TRUE;
	return pMap->m_bDlpFile;
}
BOOL ClaSecuPolicy::GetDlpClipboard()
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return TRUE;
	return pMap->m_bDlpClipboard;
}
BOOL ClaSecuPolicy::GetDlpCapture()
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return TRUE;
	return pMap->m_bDlpCapture;
}
BOOL ClaSecuPolicy::GetDlpPrintPdf()
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return TRUE;
	return pMap->m_bDlpPrintPdf;
}
BOOL ClaSecuPolicy::GetDlpPrintDev()
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return TRUE;
	return pMap->m_bDlpPrintDev;
}
BOOL ClaSecuPolicy::GetDlpGlobalFlag()
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return TRUE;
	return pMap->m_bDlpGlobal;
}
unsigned int ClaSecuPolicy::GetDlpCryptoKey(unsigned char* p_pKey)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return -1;

	memcpy(p_pKey, pMap->m_pCryptoKey, 32);

	return 0;
}

unsigned int ClaSecuPolicy::RegistCurPath()
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return -1;

	GetModuleFileName(NULL, pMap->m_wszWorkDir, MAX_PATH);
	wchar_t* pPos = wcsrchr(pMap->m_wszWorkDir, L'\\');
	pPos[0] = 0x0;

	return 0;
}
unsigned int ClaSecuPolicy::GetCurPath(wchar_t* p_wszPath)
{
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return -1;

	wcscpy_s(p_wszPath, MAX_PATH, pMap->m_wszWorkDir);

	return 0;
}

unsigned int ClaSecuPolicy::AddSecuProcess(unsigned int p_nPID)
{
	unsigned int nRet = 0;
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return -1;

	pMap->m_lPID[pMap->m_nPIDIndex] = p_nPID;
	pMap->m_nPIDIndex = (pMap->m_nPIDIndex + 1) % GD_MAX_COUNT_PID;

	return 0;
}

bool ClaSecuPolicy::IsAddSecuProcess(unsigned int p_nPID)
{
	unsigned int nRet = 0;
	ST_SECU_MAP* pMap = (ST_SECU_MAP*)OpenFileMapSecu();
	if (pMap == NULL) return false;

	for (int i = 0; i < GD_MAX_COUNT_PID; i++) {
		if (pMap->m_lPID[i] == p_nPID) {
			return true;
		}
	}
	return false;
}
