
#pragma once

/// <summary>
/// file name	: ClaProcess.h
/// to do		: define the functions related with process such as find, terminate, 
///				  run or excute, etc
/// author		: kenan
/// create		: 2024-07-10
/// modify		: 2024-07-13
/// --------------------------------------------------------------------------------
/// change log	:
/// 2024-07-10	: create
/// 2024-07-13	: add create process and return terminate code
/// </summary>

typedef void (*FN_BEFORE_INJECT)(unsigned int p_nPID);

class ClaProcess
{
public:
	ClaProcess();
	~ClaProcess();

public:
	//	p_wszName musb be lower case name
	static unsigned long FindProcessID(const wchar_t* p_wszName, int p_nSID = -1);
	static unsigned long TerminateProcessByID(unsigned long p_lPID);
	//	p_wszName musb be lower case name
	static unsigned long TerminateProcessByName(const wchar_t* p_wszName);
	static unsigned long CreateProcess_wait(
		const wchar_t* p_wszPath, const wchar_t* p_wszCmd, const wchar_t* p_wszWorkDir,
		unsigned int* p_plRetCode);
	static unsigned long CreateAndWait(
		const wchar_t* p_wszPath, const wchar_t* p_wszCmd, const wchar_t* p_wszWorkDir,
		unsigned int* p_plRetCode);
	static unsigned long InjectDLL(unsigned int p_nPID, const wchar_t* p_wszDllPath);
	static void InjectDll(HANDLE p_hProcess, LPCWSTR p_wszDllPath /*= NULL*/);

	static unsigned long CreateProcessAsTargetWithInject(
		const wchar_t* p_wszPath, const wchar_t* p_wszCmd, const wchar_t* p_wszWorkDir,
		const wchar_t* p_wszTarget, const wchar_t* p_wszDllPath, FN_BEFORE_INJECT p_fnCB
		);
	static unsigned long CreateProcessEx(const wchar_t* p_wszPath, const wchar_t* p_wszCmd, const wchar_t* p_wszWorkDir,
		DWORD * p_pdwPID, HANDLE* p_pHandle, STARTUPINFO* p_psi = NULL
		);
	static unsigned long CreateProcessWithInject(
		const wchar_t* p_wszPath, const wchar_t* p_wszCmd, const wchar_t* p_wszWorkDir,
		const wchar_t* p_wszDllPath, FN_BEFORE_INJECT p_fnCB
	);

	static unsigned long CreateProcessAsTarget(
		const wchar_t* p_wszTarget,
		const wchar_t* p_wszPath,
		const wchar_t* p_wszCmd,
		const wchar_t* p_wszWorkDir,
		DWORD p_dwCreateFlag,
		LPSTARTUPINFOW p_pSI,
		LPPROCESS_INFORMATION p_pPI
	);
	static bool IsProcess64Bit(HANDLE hProcess);
	static DWORD GetParentProcessId(DWORD processID);
};

