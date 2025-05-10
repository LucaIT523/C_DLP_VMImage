#include "pch.h"
#include "Global.h"

#define LD_LOG_HEAD_A	"Kenan-LogA : "
#define LD_LOG_HEAD_W	L"Kenan-LogW : "

char* lv_pLogA = NULL;
wchar_t* lv_pLogW = NULL;

char* lv_pLogRA = NULL;
wchar_t* lv_pLogRW = NULL;

#define MAX_LOG_LENA	4096
#define MAX_LOG_LENW	4096 * sizeof(wchar_t)

wchar_t lv_wszCurExePath[MAX_PATH];
DWORD	lv_dwPID;
HANDLE lv_mutex_log = NULL;

CRITICAL_SECTION sync_section;

BOOL lv_bLog = TRUE;
DWORD lv_dwLogLevel = 0xFFFFFFFF;
DWORD lv_dwLogArea = 0xFFFFFFFF;

void _initLog() {
	if (lv_pLogA == NULL) {
		lv_pLogA = (char*)malloc(MAX_LOG_LENA);
		lv_pLogRA = (char*)malloc(MAX_LOG_LENA);
		memset(lv_wszCurExePath, 0, sizeof(lv_wszCurExePath));

		memset(lv_pLogA, 0, MAX_LOG_LENA);
		memset(lv_pLogRA, 0, MAX_LOG_LENA);
	}
	if (lv_pLogW == NULL) {
		lv_pLogW = (wchar_t*)malloc(MAX_LOG_LENW * sizeof(wchar_t));
		lv_pLogRW = (wchar_t*)malloc(MAX_LOG_LENW * sizeof(wchar_t));

		InitializeCriticalSection(&sync_section);

		memset(lv_pLogW, 0, MAX_LOG_LENW);
		memset(lv_pLogRW, 0, MAX_LOG_LENW);
	}

	if (lv_wszCurExePath[0] == 0) {
		GetModuleFileName(NULL, lv_wszCurExePath, MAX_PATH);
		lv_dwPID = GetCurrentProcessId();
	}
}

void LogPrintW(unsigned int p_nLogLevel, unsigned int p_nLogArea, const wchar_t* p_wszFmt, ...)
{
	_initLog();

	if (lv_bLog == FALSE) {
		return;
	}
	if ((lv_dwLogArea & p_nLogArea) == 0) {
		return;
	}
	if ((lv_dwLogLevel & p_nLogLevel) == 0) {
		return;
	}

	EnterCriticalSection(&sync_section);

	va_list vl;
	va_start(vl, p_wszFmt);
	vswprintf_s(lv_pLogW, MAX_LOG_LENA, p_wszFmt, vl);
	va_end(vl);

	swprintf_s(lv_pLogRW, MAX_LOG_LENA, L"[%s][%d]---%s%s", lv_wszCurExePath, lv_dwPID, LD_LOG_HEAD_W, lv_pLogW);
	OutputDebugStringW(lv_pLogRW);

	FILE* pFile; _wfopen_s(&pFile, L"C:\\ProgramData\\kenan.log", L"a");
	if (pFile != NULL) {
		fprintf_s(pFile, "%S\n", lv_pLogRW);
		fclose(pFile);
	}

	LeaveCriticalSection(&sync_section);
}

void LogPrintA(unsigned int p_nLogLevel, unsigned int p_nLogArea, const char* p_szFmt, ...)
{
	_initLog();

	if (lv_bLog == FALSE) {
		return;
	}
	if ((lv_dwLogArea & p_nLogArea) == 0) {
		return;
	}
	if ((lv_dwLogLevel & p_nLogLevel) == 0) {
		return;
	}

	EnterCriticalSection(&sync_section);

	va_list vl;
	va_start(vl, p_szFmt);
	vsprintf_s(lv_pLogA, MAX_LOG_LENA, p_szFmt, vl);
	va_end(vl);

	sprintf_s(lv_pLogRA, MAX_LOG_LENA, "[%S][%d]---%s%s", lv_wszCurExePath, lv_dwPID, LD_LOG_HEAD_A, lv_pLogA);
	OutputDebugStringA(lv_pLogRA);

	FILE* pFile; _wfopen_s(&pFile, L"C:\\ProgramData\\kenan.log", L"a");
	if (pFile != NULL) {
		fprintf_s(pFile, "%S\n", lv_pLogRW);
		fclose(pFile);
	}

	LeaveCriticalSection(&sync_section);

}