#include "pch.h"
#include "KenanVmxHelper.h"
#include "detours.h"
#include "KGlobal.h"

typedef HANDLE(WINAPI* FN_CreateFileA)(
	_In_ LPCSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile
	);
FN_CreateFileA lv_orgCreateFileA = NULL;

typedef HANDLE(WINAPI* FN_CreateFileW)(
	_In_ LPCWSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile
	);
FN_CreateFileW lv_orgCreateFileW = NULL;

HANDLE WINAPI hook_CreateFileA(
	_In_ LPCSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile
)
{
	ClaKcConfig conf;
	KGlobal::readConfig(&conf);

	if (lpFileName != NULL && strstr(lpFileName, "VMwareDnD") != NULL) {
		if (dwDesiredAccess & GENERIC_WRITE) {
			SetLastError(ERROR_ACCESS_DENIED);
			return INVALID_HANDLE_VALUE;
		}
	}

	return lv_orgCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
		dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE WINAPI hook_CreateFileW(
	_In_ LPCWSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile
)
{
	ClaKcConfig conf;
	KGlobal::readConfig(&conf);


	if (lpFileName != NULL && wcsstr(lpFileName, L"VMwareDnD") != NULL) {
		if (dwDesiredAccess & GENERIC_WRITE) {
			SetLastError(ERROR_ACCESS_DENIED);
			return INVALID_HANDLE_VALUE;
		}
	}

	return lv_orgCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
		dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}


void hook_vmx()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	HMODULE hModKernel = LoadLibraryW(L"kernelbase.dll");
	if (hModKernel)
	{
		lv_orgCreateFileA = (FN_CreateFileA)GetProcAddress(hModKernel, "CreateFileA");
		lv_orgCreateFileW = (FN_CreateFileW)GetProcAddress(hModKernel, "CreateFileW");
	}

	if (DetourAttach(&(PVOID&)lv_orgCreateFileA, hook_CreateFileA) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgCreateFileW, hook_CreateFileW) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}

	if (DetourTransactionCommit() != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
}

void unhook_vmx()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (lv_orgCreateFileA)
	{
		if (DetourDetach(&(PVOID&)lv_orgCreateFileA, hook_CreateFileA) != NO_ERROR) {
			OutputDebugStringW(L"err");
		}
	}
	if (lv_orgCreateFileW){
		if (DetourDetach(&(PVOID&)lv_orgCreateFileW, hook_CreateFileW) != NO_ERROR) {
			OutputDebugStringW(L"err");
		}
	}

	if (DetourTransactionCommit() != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
}
