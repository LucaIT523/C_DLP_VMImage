#include "pch.h"
#include "ClaProcess.h"
#include <tlhelp32.h>

ClaProcess::ClaProcess()
{

}

ClaProcess::~ClaProcess()
{

}

unsigned long ClaProcess::FindProcessID(const wchar_t* p_wszName, int p_nSID /*= -1*/)
{
	unsigned long lRet = 0;

    PROCESSENTRY32 pe32;
    HANDLE hSnapshot = NULL;
    wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));

    pe32.dwSize = sizeof(PROCESSENTRY32);
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            memset(wszPath, 0, sizeof(wszPath));
            wcscpy_s(wszPath, MAX_PATH, pe32.szExeFile); _wcslwr_s(wszPath, MAX_PATH);
            if (_wcsicmp(wszPath, p_wszName) == 0) {
                DWORD dwSID;
                if (p_nSID != -1) {
                    ProcessIdToSessionId(pe32.th32ProcessID, &dwSID);
                    if (dwSID != p_nSID) continue;
                }
                lRet = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return lRet;
}

unsigned long ClaProcess::TerminateProcessByID(unsigned long p_lPID)
{
	unsigned long lRet = 0;
    
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, p_lPID);
    if (hProcess == NULL) {
        return -1;
    }

    TerminateProcess(hProcess, 0);

	return lRet;
}

unsigned long ClaProcess::TerminateProcessByName(const wchar_t* p_wszName)
{
    unsigned long lRet = 0;

    PROCESSENTRY32 pe32;
    HANDLE hSnapshot = NULL;
    wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));

    pe32.dwSize = sizeof(PROCESSENTRY32);
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            memset(wszPath, 0, sizeof(wszPath));
            wcscpy_s(wszPath, MAX_PATH, pe32.szExeFile); _wcslwr_s(wszPath, MAX_PATH);
            if (wcscmp(wszPath, p_wszName) == 0) {
                TerminateProcessByID(pe32.th32ProcessID);
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return lRet;
}

unsigned long ClaProcess::CreateProcess_wait(
    const wchar_t* p_wszPath,
    const wchar_t* p_wszCmd,
    const wchar_t* p_wszWorkDir,
    unsigned int* p_plRetCode)
{
    BOOL bSts = FALSE;
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    bSts = CreateProcess(p_wszPath, (LPWSTR)p_wszCmd, NULL, NULL, FALSE, 0, NULL, p_wszWorkDir, &si, &pi);
    if (bSts == FALSE) {
        return GetLastError();
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD dwExitCode;
    GetExitCodeProcess(pi.hProcess, &dwExitCode);
    *p_plRetCode = dwExitCode;
    return 0;
}


unsigned long ClaProcess::CreateAndWait(
    const wchar_t* p_wszPath,
    const wchar_t* p_wszCmd,
    const wchar_t* p_wszWordDir,
    unsigned int* p_plRetCode)
{
    BOOL bSts = FALSE;
    STARTUPINFO si = { sizeof(si) };
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW;  // Indicate that we want to control the window appearance
    si.wShowWindow = SW_HIDE;  // Hide the window
    PROCESS_INFORMATION pi;

    bSts = CreateProcess(p_wszPath, (LPWSTR)p_wszCmd, NULL, NULL, FALSE, 0, NULL, p_wszWordDir, &si, &pi);
    if (bSts == FALSE) {
        return GetLastError();
    }

    if (WaitForSingleObject(pi.hProcess, 1000) == WAIT_TIMEOUT) {
        TerminateProcess(pi.hProcess, 0);
    }

    DWORD dwExitCode;
    GetExitCodeProcess(pi.hProcess, &dwExitCode);
    *p_plRetCode = dwExitCode;
    return 0;
}


unsigned long ClaProcess::InjectDLL(unsigned int p_nPID, const wchar_t* p_wszDllPath)
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, p_nPID);
    if (hProcess == NULL) {
        return false;
    }

    InjectDll(hProcess, p_wszDllPath);

    CloseHandle(hProcess);

    return true;
}

void ClaProcess::InjectDll(HANDLE p_hProcess, LPCWSTR p_wszDllPath /*= NULL*/) {
    wchar_t wszDllPath[MAX_PATH]; memset(wszDllPath, 0, sizeof(wszDllPath));
    wcscpy_s(wszDllPath, MAX_PATH, p_wszDllPath);
    unsigned int cchDllPath = sizeof(wszDllPath);

    HMODULE hKernel32 = GetModuleHandleA("Kernelbase.dll");
    FARPROC lb = GetProcAddress(hKernel32, "LoadLibraryW");

    HANDLE hProcess = p_hProcess;

    void* pMem = VirtualAllocEx(hProcess, NULL, cchDllPath, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
    WriteProcessMemory(hProcess, pMem, wszDllPath, cchDllPath, NULL);
    HANDLE h = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lb, pMem, 0, NULL);
    WaitForSingleObject(h, 1000);
    VirtualFreeEx(hProcess, pMem, 0, MEM_RELEASE);
    CloseHandle(h);
}


bool _GetProcessToken(HANDLE& hToken, const wchar_t* p_wszTarget) {
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE || hProcessSnap == NULL) {
        return false;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        return false;
    }

    do {
        if (_wcsicmp(pe32.szExeFile, p_wszTarget) == 0) {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
            if (hProcess == NULL) {
                CloseHandle(hProcessSnap);
                return false;
            }

            if (!OpenProcessToken(hProcess, TOKEN_DUPLICATE, &hToken)) {
                CloseHandle(hProcess);
                CloseHandle(hProcessSnap);
                return false;
            }

            CloseHandle(hProcess);
            break;
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return true;
}

unsigned long ClaProcess::CreateProcessAsTargetWithInject(
    const wchar_t* p_wszPath, const wchar_t* p_wszCmd, const wchar_t* p_wszWorkDir,
    const wchar_t* p_wszTarget, const wchar_t* p_wszDllPath, FN_BEFORE_INJECT p_fnCB
)
{
    HANDLE hToken;
    if (!_GetProcessToken(hToken, p_wszTarget)) {
        return -1;
    }

    HANDLE hNewToken;
    if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &hNewToken)) {
        //		std::cerr << "DuplicateTokenEx failed: " << GetLastError() << '\n';
        CloseHandle(hToken);
        return -1;
    }

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (!CreateProcessAsUser(hNewToken, p_wszPath, (LPWSTR)p_wszCmd, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, p_wszWorkDir, &si, &pi)) {
        //		std::cerr << "CreateProcessAsUser failed: " << GetLastError() << '\n';
        CloseHandle(hNewToken);
        CloseHandle(hToken);
        return -1;
    }

    if (p_fnCB) {
        p_fnCB(pi.dwProcessId);
    }
    
    {
        wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
        wcscpy_s(wszPath, MAX_PATH, p_wszPath); _wcsupr_s(wszPath, MAX_PATH);
        if (wcsstr(wszPath, L"ACROBAT.EXE") == NULL) {
            InjectDll(pi.hProcess, p_wszDllPath);
        }
    }
    
    ResumeThread(pi.hThread);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hNewToken);
    CloseHandle(hToken);
    return 0;
}

unsigned long ClaProcess::CreateProcessWithInject(
    const wchar_t* p_wszPath, const wchar_t* p_wszCmd, const wchar_t* p_wszWorkDir,
    const wchar_t* p_wszDllPath, FN_BEFORE_INJECT p_fnCB
)
{
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (!CreateProcess(p_wszPath, (LPWSTR)p_wszCmd, NULL, NULL, FALSE, 0, NULL, p_wszWorkDir, &si, &pi)) {
        return -1;
    }

    if (p_fnCB) {
        p_fnCB(pi.dwProcessId);
    }

    InjectDll(pi.hProcess, p_wszDllPath);

    ResumeThread(pi.hThread);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}

unsigned long ClaProcess::CreateProcessEx(
    const wchar_t* p_wszPath, const wchar_t* p_wszCmd, const wchar_t* p_wszWorkDir,
    DWORD* p_dwPID, HANDLE* p_phHandle, STARTUPINFO* p_psi
)
{
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (p_psi == NULL) {
        if (!CreateProcess(p_wszPath, (LPWSTR)p_wszCmd, NULL, NULL, FALSE, 0, NULL, p_wszWorkDir, &si, &pi)) {
            return -1;
        }
    }
    else {
        if (!CreateProcess(p_wszPath, (LPWSTR)p_wszCmd, NULL, NULL, FALSE, 0, NULL, p_wszWorkDir, p_psi, &pi)) {
            return -1;
        }
    }
    if (p_dwPID != NULL) {
        *p_dwPID = pi.dwProcessId;
    }
    if (p_phHandle != NULL) {
        *p_phHandle = pi.hProcess;
    }
    return 0;
}


unsigned long ClaProcess::CreateProcessAsTarget(
    const wchar_t* p_wszTarget,
    const wchar_t* p_wszPath,
    const wchar_t* p_wszCmd,
    const wchar_t* p_wszWorkDir,
    DWORD p_dwCreateFlag,
    LPSTARTUPINFOW p_pSI,
    LPPROCESS_INFORMATION p_pPI
)
{
    HANDLE hToken;
    if (!_GetProcessToken(hToken, p_wszTarget)) {
        return -1;
    }

    HANDLE hNewToken;
    if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &hNewToken)) {
        //		std::cerr << "DuplicateTokenEx failed: " << GetLastError() << '\n';
        CloseHandle(hToken);
        return -1;
    }

    if (!CreateProcessAsUserW(hNewToken, p_wszPath, (LPWSTR)p_wszCmd, NULL, NULL, FALSE, p_dwCreateFlag, NULL, p_wszWorkDir, p_pSI, p_pPI)) {
        //		std::cerr << "CreateProcessAsUser failed: " << GetLastError() << '\n';
        CloseHandle(hNewToken);
        CloseHandle(hToken);
        return -1;
    }

    CloseHandle(hNewToken);
    CloseHandle(hToken);
    return 0;
}

bool ClaProcess::IsProcess64Bit(HANDLE hProcess) {
    BOOL bIsWow64 = FALSE;
    if (IsWow64Process(hProcess, &bIsWow64)) {
        return !bIsWow64;
    }
    else {
        return false;
    }
}

DWORD ClaProcess::GetParentProcessId(DWORD processID) {
    if (processID == 0) {
        processID = GetCurrentProcessId(); 
    }

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0; // Error creating snapshot
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    // Iterate through the processes
    if (Process32First(hSnapshot, &pe)) {
        do {
            if (pe.th32ProcessID == processID) {
                CloseHandle(hSnapshot);
                return pe.th32ParentProcessID; // Return the parent process ID
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return 0; // Process not found
}
