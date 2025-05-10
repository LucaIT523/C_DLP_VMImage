#include "pch.h"
#include "KenanHelperVM.h"
#include "KenanVmxRedirector.h"
#include "kncrypto.h"
#include "ClaRegMgr.h"
#include "ClaPathMgr.h"

#include "detours.h"

#include <winternl.h>
#include <ntstatus.h>

void _encryptFileW(const wchar_t* p_wszPathTemp, const wchar_t* p_wszPathOrg);
void _decryptFileW(const wchar_t* p_wszPathOrg, const wchar_t* p_wszPathTemp);
__int64 _get_file_size(const wchar_t* p_wszPath);
int _read_file(const wchar_t* p_wszPath, void* p_pBuff, int p_nLen);
int _write_file(const wchar_t* p_wszPath, void* p_pBuff, int p_nLen);
int _delete_file(const wchar_t* p_wszPath);
void _encryptBuffer(void* p_pBuff, int p_nLen);
void _decryptBuffer(void* p_pBuff, int p_nLen);

void ss_insert_handleW(HANDLE p_hFile, const wchar_t* p_wszPathOrg, const wchar_t* p_wszTemp);
BOOL ss_get_handle(HANDLE p_hFile, wchar_t* p_wszPathOrg, wchar_t* p_wszTemp);
BOOL ss_exist_file(const wchar_t* p_wszPathOrg);
void ss_remove_handle(HANDLE p_hFile);
void add_open_handle(HANDLE p_hHandle);
BOOL is_exist_open_handle(HANDLE p_hHandle);
void remove_open_handle(HANDLE p_hHandle);

CString lv_strVMX = L"";
CString lv_strDVMX = L"";

BOOL isTargetPath(const wchar_t* p_wszPath) {
	CString str = p_wszPath;
	if (str.Right(4).CompareNoCase(L".vmx") == 0) return TRUE;
	return FALSE;
}
CString getTargetPath(const wchar_t* p_wszPath) {
	//CString strPath = p_wszPath;
	//CString strTemp = L"\\??\\C:\\Windows\\Web\\Wallpaper\\wallpaper.db";
	//strPath.Replace(lv_strVMX, strTemp);
	CString strRet = p_wszPath;
	strRet.Replace(L".vmx", L".vml");
	return strRet;
}

typedef struct _FILE_STANDARD_INFORMATION {
	LARGE_INTEGER AllocationSize;
	LARGE_INTEGER EndOfFile;
	ULONG NumberOfLinks;
	BOOLEAN DeletePending;
	BOOLEAN Directory;
} FILE_STANDARD_INFORMATION, * PFILE_STANDARD_INFORMATION;

typedef struct _FILE_RENAME_INFORMATION {
	BOOLEAN ReplaceIfExists; // Set to TRUE to replace a file with the same name, FALSE otherwise
	HANDLE RootDirectory;    // Optional handle to the root directory for relative paths
	ULONG FileNameLength;    // Length of the new name in bytes
	WCHAR FileName[1];       // New name for the file (variable length)
} FILE_RENAME_INFORMATION, * PFILE_RENAME_INFORMATION;

//
typedef NTSTATUS(NTAPI* FN_NtCreateFile)(
	OUT PHANDLE FileHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock, IN PLARGE_INTEGER AllocationSize OPTIONAL,
	IN ULONG FileAttributes, IN ULONG ShareAccess, IN ULONG CreateDisposition,
	IN ULONG CreateOptions, IN PVOID EaBuffer OPTIONAL, IN ULONG EaLength);
FN_NtCreateFile lv_orgNtCreateFile = NULL;

//
typedef NTSTATUS(NTAPI* FN_NtClose)(HANDLE);
FN_NtClose lv_orgNtClose = NULL;

//
typedef NTSTATUS(NTAPI* FN_NtOpenFile)(
	PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock, ULONG ShareAccess, ULONG OpenOptions);
FN_NtOpenFile lv_orgZwOpenFile = NULL;

//
typedef NTSTATUS(NTAPI* FN_NtReadFile)(HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID, PIO_STATUS_BLOCK, PVOID, ULONG, PLARGE_INTEGER, PULONG);
FN_NtReadFile lv_orgNtReadFile = NULL;

//
typedef NTSTATUS(NTAPI* FN_NtWriteFile)(HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID,PIO_STATUS_BLOCK, PVOID, ULONG,PLARGE_INTEGER, PULONG);
FN_NtWriteFile lv_orgNtWriteFile = NULL;

//
typedef NTSTATUS(NTAPI* FN_NtQueryInformationFile)(HANDLE FileHandle, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation, ULONG Length,FILE_INFORMATION_CLASS FileInformationClass);
FN_NtQueryInformationFile lv_orgNtQueryInformationFile = NULL;

//
typedef NTSTATUS(NTAPI* FN_NtSetInformationFile)(HANDLE FileHandle, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation, ULONG Length, FILE_INFORMATION_CLASS FileInformationClass);
FN_NtSetInformationFile lv_orgNtSetInformationFile = NULL;

//
typedef NTSTATUS(NTAPI* FN_NtDeleteFile)(PIO_STATUS_BLOCK IoStatusBlock, POBJECT_ATTRIBUTES ObjectAttributes);
FN_NtDeleteFile lv_orgNtDeleteFile = NULL;

typedef DWORD (WINAPI*FN_GetFileAttributesW)(_In_ LPCWSTR lpFileName);
FN_GetFileAttributesW lv_orgGetFileAttributesW = NULL;

typedef BOOL (WINAPI*FN_GetFileAttributesExW)(
	_In_ LPCWSTR lpFileName,
	_In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
	_Out_writes_bytes_(sizeof(WIN32_FILE_ATTRIBUTE_DATA)) LPVOID lpFileInformation
);
FN_GetFileAttributesExW lv_orgGetFileAttributesExW;

typedef HANDLE (WINAPI*FN_FindFirstFileExW)(
	_In_ LPCWSTR lpFileName,
	_In_ FINDEX_INFO_LEVELS fInfoLevelId,
	_Out_writes_bytes_(sizeof(WIN32_FIND_DATAW)) LPVOID lpFindFileData,
	_In_ FINDEX_SEARCH_OPS fSearchOp,
	_Reserved_ LPVOID lpSearchFilter,
	_In_ DWORD dwAdditionalFlags
);
FN_FindFirstFileExW lv_orgFindFirstFileExW;

/////////////////////////////////////////////////////////////////////////////////////////


NTSTATUS NTAPI hook_NtCreateFile(
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
		return lv_orgNtCreateFile(FileHandle, DesiredAccess, ObjectAttributes,
			IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition,
			CreateOptions, EaBuffer, EaLength);
	}

	if (ObjectAttributes && ObjectAttributes->ObjectName && ObjectAttributes->ObjectName->Buffer) {
		wcscpy_s(wszPath, MAX_PATH, ObjectAttributes->ObjectName->Buffer);
		if (isTargetPath(wszPath)) {
			CString strPath = getTargetPath(wszPath);

			OBJECT_ATTRIBUTES objAttr;
			UNICODE_STRING fileName;
			wchar_t wszPathT[MAX_PATH]; memset(wszPathT, 0, sizeof(wszPathT));
			wcscpy_s(wszPathT, MAX_PATH, strPath);

			RtlInitUnicodeString(&fileName, wszPathT);
			InitializeObjectAttributes(&objAttr, &fileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

			return lv_orgNtCreateFile(FileHandle, DesiredAccess, &objAttr,
				IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition,
				CreateOptions, EaBuffer, EaLength);
		}
	}
	return lv_orgNtCreateFile(FileHandle, DesiredAccess, ObjectAttributes,
		IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition,
		CreateOptions, EaBuffer, EaLength);
}

NTSTATUS NTAPI hook_NtOpenFile(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions
)
{
	BOOL isTarget = FALSE;
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
	CString strPath;
	CString strPathT;
	wchar_t* pPos;
	wchar_t wszHead[32];
	CString strDecrypted;

	if (ObjectAttributes == NULL || ObjectAttributes->ObjectName == NULL || (OpenOptions & FILE_DIRECTORY_FILE)) {
		return lv_orgZwOpenFile(FileHandle, DesiredAccess, ObjectAttributes,
			IoStatusBlock, ShareAccess, OpenOptions);
	}

	if (ObjectAttributes && ObjectAttributes->ObjectName && ObjectAttributes->ObjectName->Buffer) {
		wcscpy_s(wszPath, MAX_PATH, ObjectAttributes->ObjectName->Buffer);
		if (isTargetPath(wszPath)) {
			CString strPath = getTargetPath(wszPath);

			OBJECT_ATTRIBUTES objAttr;
			UNICODE_STRING fileName;
			wchar_t wszPathT[MAX_PATH]; memset(wszPathT, 0, sizeof(wszPathT));
			wcscpy_s(wszPathT, MAX_PATH, strPath);

			RtlInitUnicodeString(&fileName, wszPathT);
			InitializeObjectAttributes(&objAttr, &fileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

			return lv_orgZwOpenFile(FileHandle, DesiredAccess, &objAttr,
				IoStatusBlock, ShareAccess, OpenOptions);
		}
	}
	return lv_orgZwOpenFile(FileHandle, DesiredAccess, ObjectAttributes,
		IoStatusBlock, ShareAccess, OpenOptions);
}

BOOL NTAPI hook_NtClose(HANDLE hObject)
{
	return lv_orgNtClose(hObject);
}

NTSTATUS NTAPI hook_NtSetInformationFile(
	HANDLE                 FileHandle,
	PIO_STATUS_BLOCK       IoStatusBlock,
	PVOID                  FileInformation,
	ULONG                  Length,
	FILE_INFORMATION_CLASS FileInformationClass
)
{
	if (FileInformationClass == 10/*FileRenameInformation*/) {
		//.	oepn secu file
		FILE_RENAME_INFORMATION* renameInfo = (FILE_RENAME_INFORMATION*)FileInformation;
		if (renameInfo && renameInfo->FileName) {
			if (isTargetPath(renameInfo->FileName)) {
				CString strTargetPath = getTargetPath(renameInfo->FileName);
				FILE_RENAME_INFORMATION *renameInfoTarget = (FILE_RENAME_INFORMATION*)malloc(sizeof(FILE_RENAME_INFORMATION) + 520);
				memset(renameInfoTarget, 0, sizeof(FILE_RENAME_INFORMATION) + 520);
				wcscpy_s(renameInfoTarget->FileName, 260, strTargetPath);
				renameInfoTarget->FileNameLength = wcslen(renameInfoTarget->FileName);
				renameInfoTarget->ReplaceIfExists = renameInfo->ReplaceIfExists;
				renameInfoTarget->RootDirectory = renameInfo->RootDirectory;

				NTSTATUS ret = lv_orgNtSetInformationFile(
					FileHandle,
					IoStatusBlock,
					renameInfoTarget,
					sizeof(FILE_RENAME_INFORMATION) + 520,
					FileInformationClass
				);
				free(renameInfoTarget);
				return ret;
			}
		}

	}
	return lv_orgNtSetInformationFile(
		FileHandle,
		IoStatusBlock,
		FileInformation,
		Length,
		FileInformationClass
	);
}

DWORD WINAPI hook_GetFileAttributesW(_In_ LPCWSTR lpFileName)
{
	if (isTargetPath(lpFileName)) {
		CString strPath = getTargetPath(lpFileName);		
		return lv_orgGetFileAttributesW(strPath);
	}
	return lv_orgGetFileAttributesW(lpFileName);
}

BOOL WINAPI hook_GetFileAttributesExW(
	_In_ LPCWSTR lpFileName,
	_In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
	_Out_writes_bytes_(sizeof(WIN32_FILE_ATTRIBUTE_DATA)) LPVOID lpFileInformation
)
{
	if (isTargetPath(lpFileName)) {
		CString strPath = getTargetPath(lpFileName);
		return lv_orgGetFileAttributesExW(strPath, fInfoLevelId, lpFileInformation);
	}
	return lv_orgGetFileAttributesExW(lpFileName, fInfoLevelId, lpFileInformation);
}

HANDLE WINAPI hook_FindFirstFileExW(
	_In_ LPCWSTR lpFileName,
	_In_ FINDEX_INFO_LEVELS fInfoLevelId,
	_Out_writes_bytes_(sizeof(WIN32_FIND_DATAW)) LPVOID lpFindFileData,
	_In_ FINDEX_SEARCH_OPS fSearchOp,
	_Reserved_ LPVOID lpSearchFilter,
	_In_ DWORD dwAdditionalFlags
	)
{
	CString strPath = lpFileName;
	if (isTargetPath(lpFileName)) {
		CString strPath = getTargetPath(lpFileName);
		return lv_orgFindFirstFileExW(strPath, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
	}

	return lv_orgFindFirstFileExW(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
}

void _encryptFileW(const wchar_t* p_wszPathTemp, const wchar_t* p_wszPathOrg)
{
	__int64 nSize = _get_file_size(p_wszPathTemp);
	if (nSize == -1) {
		DeleteFile(p_wszPathOrg); 
		CString strMsg; strMsg.Format(L"SfT - delete on enc [%s]", p_wszPathOrg);
		OutputDebugString(strMsg);
		return;
	}

	if (nSize == 0) {
		HANDLE hFile;
		OBJECT_ATTRIBUTES objAttr;
		IO_STATUS_BLOCK ioStatusBlock;
		UNICODE_STRING fileName;

		RtlInitUnicodeString(&fileName, p_wszPathOrg);
		InitializeObjectAttributes(&objAttr, &fileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

		NTSTATUS status = lv_orgNtCreateFile(
			&hFile,
			FILE_GENERIC_WRITE,
			&objAttr,
			&ioStatusBlock,
			NULL,
			FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			FILE_CREATE,
			FILE_SYNCHRONOUS_IO_NONALERT,
			NULL,
			0
		);
		if (NT_SUCCESS(status)) {
			lv_orgNtClose(hFile);
		}
		return;
	}

	unsigned char pHead[] = { 0x65, 0x87, 0x65 };
	unsigned char* pBuff = (unsigned char*)malloc(nSize);
	int nRead = _read_file(p_wszPathTemp, pBuff, nSize);
	//	if crypted file
	_encryptBuffer(pBuff, nSize);
	_write_file(p_wszPathOrg, pBuff, nSize);
	free(pBuff);
}

void _decryptFileW(const wchar_t* p_wszPathOrg, const wchar_t* p_wszPathTemp)
{
	wchar_t wszPathTemp[MAX_PATH]; memset(wszPathTemp, 0, sizeof(wszPathTemp));
	swprintf_s(wszPathTemp, MAX_PATH, L"%s.tmp", p_wszPathOrg);

	//.	if file not exist, return only temp path

	__int64 nSize = _get_file_size(p_wszPathOrg);
	if (nSize == -1) {
//		DeleteFile(p_wszPathOrg);
		return;
	}

	if (nSize == 0) {
		//.	if the file size is 0, as empty file
		HANDLE hFile;
		OBJECT_ATTRIBUTES objAttr;
		IO_STATUS_BLOCK ioStatusBlock;
		UNICODE_STRING fileName;

		RtlInitUnicodeString(&fileName, p_wszPathTemp);
		InitializeObjectAttributes(&objAttr, &fileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

		NTSTATUS status = lv_orgNtCreateFile(
			&hFile,
			FILE_GENERIC_WRITE,
			&objAttr,
			&ioStatusBlock,
			NULL,
			FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			FILE_CREATE,
			FILE_SYNCHRONOUS_IO_NONALERT,
			NULL,
			0
		);
		if (NT_SUCCESS(status)) {
			lv_orgNtClose(hFile);
		}
		return;
	}

	unsigned char* pBuff = (unsigned char*)malloc(nSize); memset(pBuff, 0, nSize);
	int nRead = _read_file(p_wszPathOrg, pBuff, nSize);
	//	if crypted file
	_decryptBuffer(pBuff, nSize);
	_write_file(p_wszPathTemp, pBuff, nRead);

	//unsigned char pHead[] = { 0x65, 0x87, 0x65 };
	//unsigned char* pBuff = (unsigned char*)malloc(nSize); memset(pBuff, 0, nSize);
	//int nRead = _read_file(p_wszPathOrg, pBuff, nSize);
	//if (nRead >= 3 && memcmp(pBuff, pHead, 3) == 0) {
	//	//	if crypted file
	//	_decryptBuffer(&pBuff[3], nSize - 3);
	//	_write_file(p_wszPathTemp, &pBuff[3], nRead - 3);
	//}
	//else {
	//	_write_file(p_wszPathTemp, pBuff, nRead);
	//}

	free(pBuff);
}

__int64 _get_file_size(const wchar_t* p_wszPath) {

	UNICODE_STRING fileName;
	RtlInitUnicodeString(&fileName, p_wszPath);

	OBJECT_ATTRIBUTES objAttr;
	InitializeObjectAttributes(&objAttr, &fileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	HANDLE hFile;
	IO_STATUS_BLOCK ioStatusBlock;

	NTSTATUS status = lv_orgNtCreateFile(
		&hFile,
		FILE_GENERIC_READ,
		&objAttr,
		&ioStatusBlock,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (status != STATUS_SUCCESS) {
		return -1;
	}

	FILE_STANDARD_INFORMATION fileInfo;

	status = lv_orgNtQueryInformationFile(
		hFile,
		&ioStatusBlock,
		&fileInfo,
		sizeof(fileInfo),
		(FILE_INFORMATION_CLASS)5//FileStandardInformation
	);
	lv_orgNtClose(hFile);

	if (status != STATUS_SUCCESS) {
		return -1;
	}

	return fileInfo.EndOfFile.QuadPart;

}

int _read_file(const wchar_t* p_wszPath, void* p_pBuff, int p_nLen) {
	HANDLE hFile;
	OBJECT_ATTRIBUTES objAttr;
	IO_STATUS_BLOCK ioStatusBlock;
	UNICODE_STRING filePath;

	RtlInitUnicodeString(&filePath, p_wszPath);
	InitializeObjectAttributes(&objAttr, &filePath, OBJ_CASE_INSENSITIVE, NULL, NULL);

	NTSTATUS status = lv_orgNtCreateFile(
		&hFile,
		FILE_GENERIC_READ,
		&objAttr,
		&ioStatusBlock,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (status != STATUS_SUCCESS) {
		return 0;
	}

	DWORD bytesRead = 0;
	status = lv_orgNtReadFile(
		hFile,
		NULL,
		NULL,
		NULL,
		&ioStatusBlock,
		p_pBuff,
		p_nLen,
		NULL,
		0
	);

	lv_orgNtClose(hFile);

	return p_nLen;
}

int _write_file(const wchar_t* p_wszPath, void* p_pBuff, int p_nLen) {
	HANDLE hFile;
	OBJECT_ATTRIBUTES objAttr;
	IO_STATUS_BLOCK ioStatusBlock;
	UNICODE_STRING filePath;

	RtlInitUnicodeString(&filePath, p_wszPath);
	InitializeObjectAttributes(&objAttr, &filePath, OBJ_CASE_INSENSITIVE, NULL, NULL);

	NTSTATUS status = lv_orgNtCreateFile(
		&hFile,
		FILE_GENERIC_WRITE,
		&objAttr,
		&ioStatusBlock,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OVERWRITE_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (status != STATUS_SUCCESS) {
		return 0;
	}

	DWORD bytesWritten = 0;
	status = lv_orgNtWriteFile(
		hFile,
		NULL,
		NULL,
		NULL,
		&ioStatusBlock,
		p_pBuff,
		p_nLen,
		NULL,
		0
	);

	lv_orgNtClose(hFile);

	return p_nLen;
}

int _delete_file(const wchar_t* p_wszPath) {
	DeleteFile(&p_wszPath[4]);
	return 0;
	// Initialize OBJECT_ATTRIBUTES
	OBJECT_ATTRIBUTES objAttr;
	UNICODE_STRING unicodeString;
	IO_STATUS_BLOCK ioStatusBlock;

	RtlInitUnicodeString(&unicodeString, p_wszPath);
	InitializeObjectAttributes(&objAttr, &unicodeString, OBJ_CASE_INSENSITIVE, NULL, NULL);

	// Call NtDeleteFile
	NTSTATUS status = lv_orgNtDeleteFile(&ioStatusBlock, &objAttr);
	return status;
}

void _encryptBuffer(void* p_pBuff, int p_nLen) {

	kn_encrypt_with_pwd((unsigned char*)p_pBuff, p_nLen, L"pwd-1");
}

void _decryptBuffer(void* p_pBuff, int p_nLen) {
	kn_decrypt_with_pwd((unsigned char*)p_pBuff, p_nLen, L"pwd-1");
}

typedef struct tagHandleInfo {
	HANDLE m_hHandle;
	BOOL m_bWide;
	wchar_t m_wszPathSrc[MAX_PATH];
	wchar_t m_wszPathTemp[60];
}ST_HANDLE_INFO;
CArray<ST_HANDLE_INFO> lv_handleList;

CArray<HANDLE> lv_handleOpenList;

void ss_insert_handleW(HANDLE p_hFile, const wchar_t* p_wszPathOrg, const wchar_t* p_wszTemp)
{
	if (p_hFile == NULL || p_hFile == INVALID_HANDLE_VALUE) return;

	ST_HANDLE_INFO stInfo; memset(&stInfo, 0, sizeof(stInfo));

	stInfo.m_hHandle = p_hFile;
	stInfo.m_bWide = TRUE;
	wcscpy_s(stInfo.m_wszPathSrc, MAX_PATH, p_wszPathOrg);
	wcscpy_s(stInfo.m_wszPathTemp, 60, p_wszTemp);

	lv_handleList.Add(stInfo);
}

BOOL ss_get_handle(HANDLE p_hFile, wchar_t* p_wszPathOrg, wchar_t* p_wszTemp)
{
	if (p_hFile == NULL || p_hFile == INVALID_HANDLE_VALUE) return FALSE;

	for (int i = 0; i < lv_handleList.GetCount(); i++) {
		if (lv_handleList[i].m_hHandle == p_hFile) {
			wcscpy_s(p_wszPathOrg, MAX_PATH, lv_handleList[i].m_wszPathSrc);
			wcscpy_s(p_wszTemp, MAX_PATH, lv_handleList[i].m_wszPathTemp);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ss_exist_file(const wchar_t* p_wszPathOrg)
{
	for (int i = 0; i < lv_handleList.GetCount(); i++) {
		if (wcscmp(p_wszPathOrg, lv_handleList[i].m_wszPathSrc) == 0) {
			return TRUE;
		}
	}
	return FALSE;
}

void ss_remove_handle(HANDLE p_hFile)
{
	for (int i = 0; i < lv_handleList.GetCount(); i++) {
		if (lv_handleList[i].m_hHandle == p_hFile) {
			lv_handleList.RemoveAt(i);
			break;
		}
	}
}

void add_open_handle(HANDLE p_hHandle) {
	lv_handleOpenList.Add(p_hHandle);
}
BOOL is_exist_open_handle(HANDLE p_hHandle) {
	for (int i = 0; i < lv_handleOpenList.GetCount(); i++) {
		if (lv_handleOpenList[i] == p_hHandle)
			return TRUE;
	}
	return FALSE;
}
void remove_open_handle(HANDLE p_hHandle) {
	for (int i = 0; i < lv_handleOpenList.GetCount(); i++) {
		if (lv_handleOpenList[i] == p_hHandle) {
			lv_handleOpenList.RemoveAt(i);
			return;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void hook_redirector() {
	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	wchar_t wszVmxPath[260];
	reg.readStringW(L"Software\\kenan", L"vmx", wszVmxPath, 260);
	lv_strVMX = wszVmxPath;
	
	wchar_t wszDVmxPath[260];
	reg.readStringW(L"Software\\kenan", L"dvmx", wszDVmxPath, 260);
	lv_strDVMX = wszDVmxPath;


	HMODULE hModNtDll = LoadLibraryW(L"ntdll.dll");
	lv_orgNtCreateFile = (FN_NtCreateFile)GetProcAddress(hModNtDll, "NtCreateFile");
	lv_orgNtReadFile = (FN_NtReadFile)GetProcAddress(hModNtDll, "NtReadFile");
	lv_orgNtWriteFile = (FN_NtWriteFile)GetProcAddress(hModNtDll, "NtWriteFile");
	lv_orgNtClose = (FN_NtClose)GetProcAddress(hModNtDll, "NtClose");
	lv_orgNtQueryInformationFile = (FN_NtQueryInformationFile)GetProcAddress(hModNtDll, "NtQueryInformationFile");
	lv_orgZwOpenFile = (FN_NtOpenFile)GetProcAddress(hModNtDll, "NtOpenFile");
	lv_orgNtSetInformationFile = (FN_NtSetInformationFile)GetProcAddress(hModNtDll, "NtSetInformationFile");
	lv_orgNtDeleteFile = (FN_NtDeleteFile)GetProcAddress(hModNtDll, "NtDeleteFile");

	HMODULE hModKernelBase = LoadLibraryW(L"kernelbase.dll");
	lv_orgGetFileAttributesW = (FN_GetFileAttributesW)GetProcAddress(hModKernelBase, "GetFileAttributesW");
	lv_orgGetFileAttributesExW = (FN_GetFileAttributesExW)GetProcAddress(hModKernelBase, "GetFileAttributesExW");
	lv_orgFindFirstFileExW = (FN_FindFirstFileExW)GetProcAddress(hModKernelBase, "FindFirstFileExW");

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (DetourAttach(&(PVOID&)lv_orgNtCreateFile, hook_NtCreateFile) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgZwOpenFile, hook_NtOpenFile) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgNtClose, hook_NtClose) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgNtSetInformationFile, hook_NtSetInformationFile) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgGetFileAttributesW, hook_GetFileAttributesW) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgGetFileAttributesExW, hook_GetFileAttributesExW) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgFindFirstFileExW, hook_FindFirstFileExW) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}

	if (DetourTransactionCommit() != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
}

void unhook_redirector() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (lv_orgNtCreateFile && DetourDetach(&(PVOID&)lv_orgNtCreateFile, hook_NtCreateFile) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (lv_orgZwOpenFile && DetourDetach(&(PVOID&)lv_orgZwOpenFile, hook_NtOpenFile) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (lv_orgNtClose && DetourDetach(&(PVOID&)lv_orgNtClose, hook_NtClose) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (lv_orgNtSetInformationFile && DetourDetach(&(PVOID&)lv_orgNtSetInformationFile, hook_NtSetInformationFile) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (lv_orgGetFileAttributesW && DetourDetach(&(PVOID&)lv_orgGetFileAttributesW, hook_GetFileAttributesW) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (lv_orgGetFileAttributesExW && DetourDetach(&(PVOID&)lv_orgGetFileAttributesExW, hook_GetFileAttributesExW) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (lv_orgFindFirstFileExW && DetourDetach(&(PVOID&)lv_orgFindFirstFileExW, hook_FindFirstFileExW) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}

	if (DetourTransactionCommit() != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
}

