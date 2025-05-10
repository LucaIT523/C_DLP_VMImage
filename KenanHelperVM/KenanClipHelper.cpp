#include "pch.h"
#include "KenanHelperVM.h"
#include "KenanClipHelper.h"
#include "KGlobal.h"
#include "detours.h"

typedef HANDLE(WINAPI* FN_SetClipboardData)(_In_ UINT uFormat, _In_opt_ HANDLE hMem);
FN_SetClipboardData lv_orgSetClipboardData = NULL;
typedef HANDLE(WINAPI* FN_GetClipboardData)(_In_ UINT uFormat);
FN_GetClipboardData lv_orgGetClipboardData = NULL;

HANDLE WINAPI hook_SetClipboardData(_In_ UINT uFormat, _In_opt_ HANDLE hMem)
{
	KGlobal::writeClipState(g_isSecuProc);

	CString strOutput;
	//if (g_isSecuProc)
	//	strOutput.Format(L"SfT - setclipboarddata from secure process[%d]", uFormat);
	//else
	//	strOutput.Format(L"SfT - setclipboarddata from non-secure process[%d]", uFormat);
	//OutputDebugString(strOutput);
	return lv_orgSetClipboardData(uFormat, hMem);
}

bool IsFileHandle(HANDLE hMem) {
	// Check if the handle is a file
	DWORD fileType = GetFileType(hMem);
	return (fileType != FILE_TYPE_UNKNOWN && fileType != FILE_TYPE_CHAR && fileType != FILE_TYPE_PIPE);
}

int CheckClipboardContentType(BOOL& bText, BOOL& bFile) {
	int ret = 0;
	wchar_t wszDesc[MAX_PATH];

	if (1/*OpenClipboard(NULL)*/) {
		UINT format = 0;
		bText = false;
		bFile = false;

		// Enumerate through all clipboard formats
		while ((format = EnumClipboardFormats(format))) {
			//CString s; s.Format(L"SfT - cformat = %d", format);
			//OutputDebugString(s);
			if (format == CF_HDROP || format == 49258) {
				bFile = true; // Clipboard contains files
			}
			else if (format == CF_TEXT || format == CF_UNICODETEXT) {
				bText = true; // Clipboard contains text
			}

			memset(wszDesc, 0, sizeof(wszDesc));
			if (wcsstr(wszDesc, L"FileGroupDescriptor") != NULL ||
				wcsstr(wszDesc, L"FileContents") != NULL) {
				bFile = true; // Clipboard contains files
			}

		}

		//		CloseClipboard();
	}
	else {
		//		std::cerr << "Failed to open clipboard." << std::endl;
	}
	bText = !bFile;
	return ret;
}

HANDLE WINAPI hook_GetClipboardData(_In_ UINT uFormat)
{
	BOOL bClipStatus;
	ClaKcConfig conf;
	KGlobal::readConfig(&conf);
	KGlobal::readClipState(bClipStatus);

	CString strOutput;
	OutputDebugString(strOutput);

	BOOL isText = 0;
	BOOL isFile = 0;
	int t = CheckClipboardContentType(isText, isFile);

	//strOutput.Format(L"SfT - getclip %d --> pol = [%d, %d, %d, %d, %d], s = %d--%d t = %d, f=%d",
	//	GetCurrentProcessId(),
	//	conf._bAllowTextToVM, conf._bAllowTextFromVM, conf._bAllowFileToVM,
	//	conf._bAllowFileFromVM, conf._bAllowCaptureScreen,
	//	bClipStatus, uFormat, isText, isFile
	//);
	//OutputDebugString(strOutput);

	if (g_isSecuProc) {
		if ((conf._bAllowTextToVM && isText)
			|| (conf._bAllowFileToVM && isFile)) {
			return lv_orgGetClipboardData(uFormat);
		}
	}
	else {
		if (bClipStatus == FALSE) {
			return lv_orgGetClipboardData(uFormat);
		}
		if ((conf._bAllowTextFromVM && isText)
			|| (conf._bAllowFileFromVM && isFile)) {
			return lv_orgGetClipboardData(uFormat);
		}
	}

	SetLastError(ERROR_ACCESS_DENIED);
	return NULL;
}

void hook_clipboard()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	HMODULE hModUser32 = LoadLibrary(L"user32.dll");
	lv_orgSetClipboardData = (FN_SetClipboardData)GetProcAddress(hModUser32, "SetClipboardData");
	lv_orgGetClipboardData = (FN_GetClipboardData)GetProcAddress(hModUser32, "GetClipboardData");

	if (DetourAttach(&(PVOID&)lv_orgSetClipboardData, hook_SetClipboardData) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgGetClipboardData, hook_GetClipboardData) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}

	if (DetourTransactionCommit() != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
}
void unhook_clipboard()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (lv_orgSetClipboardData)
	{
		if (DetourDetach(&(PVOID&)lv_orgSetClipboardData, hook_SetClipboardData) != NO_ERROR) {
			OutputDebugStringW(L"err");
		}
	}
	if (lv_orgGetClipboardData)
	{
		if (DetourDetach(&(PVOID&)lv_orgGetClipboardData, hook_GetClipboardData) != NO_ERROR) {
			OutputDebugStringW(L"err");
		}
	}

	if (DetourTransactionCommit() != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
}

#include <winternl.h>
#include <ntstatus.h>

typedef NTSTATUS(NTAPI* FN_NtCreateFile_CLP)(
	OUT PHANDLE FileHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock, IN PLARGE_INTEGER AllocationSize OPTIONAL,
	IN ULONG FileAttributes, IN ULONG ShareAccess, IN ULONG CreateDisposition,
	IN ULONG CreateOptions, IN PVOID EaBuffer OPTIONAL, IN ULONG EaLength);
FN_NtCreateFile_CLP lv_orgNtCreateFile_CLP = NULL;

NTSTATUS NTAPI hook_NtCreateFile_CLP(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PLARGE_INTEGER AllocationSize OPTIONAL,
	IN ULONG FileAttributes,
	IN ULONG ShareAccess,
	IN ULONG CreateDisposition,
	IN ULONG CreateOptions,
	IN PVOID EaBuffer OPTIONAL,
	IN ULONG EaLength
)
{
	BOOL isTarget = FALSE;
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
	CString strPath;
	CString strPathT;
	wchar_t* pPos;
	wchar_t wszHead[32];
	CString strDecrypted;

	if (ObjectAttributes == NULL || ObjectAttributes->ObjectName == NULL || (CreateOptions & FILE_DIRECTORY_FILE)) {
		return lv_orgNtCreateFile_CLP(FileHandle, DesiredAccess, ObjectAttributes,
			IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition,
			CreateOptions, EaBuffer, EaLength);
	}

	if (ObjectAttributes && ObjectAttributes->ObjectName && ObjectAttributes->ObjectName->Buffer) {
		CString strFile = ObjectAttributes->ObjectName->Buffer;
		if (strFile.Right(4).CompareNoCase(L".vml") == 0) {
			return STATUS_OBJECT_NAME_NOT_FOUND;
		}
	}
	return lv_orgNtCreateFile_CLP(FileHandle, DesiredAccess, ObjectAttributes,
		IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition,
		CreateOptions, EaBuffer, EaLength);
}

void hook_clipboard_vmx()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	HMODULE hModNtDll = LoadLibraryW(L"ntdll.dll");
	lv_orgNtCreateFile_CLP = (FN_NtCreateFile_CLP)GetProcAddress(hModNtDll, "NtCreateFile");

	if (DetourAttach(&(PVOID&)lv_orgNtCreateFile_CLP, hook_NtCreateFile_CLP) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}

	if (DetourTransactionCommit() != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
}

void unhook_clipboard_vmx() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (lv_orgNtCreateFile_CLP)
	{
		if (DetourDetach(&(PVOID&)lv_orgNtCreateFile_CLP, hook_NtCreateFile_CLP) != NO_ERROR) {
			OutputDebugStringW(L"err");
		}
	}

	if (DetourTransactionCommit() != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
}
