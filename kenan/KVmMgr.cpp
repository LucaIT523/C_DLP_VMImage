#include "pch.h"
#include "KVmMgr.h"
#include "detours.h"
#include "ClaProcess.h"
#include "ClaPathMgr.h"
#include "kncrypto.h"
#include "ClaRegMgr.h"

#ifndef WIN64
#include "vix.h"
#ifdef _DEBUG
#pragma comment(lib, "vixd.lib")
#else
#pragma comment(lib, "vix.lib")
#endif 
#endif

#pragma comment(lib, "detours.lib")

#ifndef WIN64

VixHandle lv_handleVM = NULL;
VixHandle lv_propertyHandle = NULL;
HANDLE lv_thdVmProgress = NULL;
KVmMgr* lv_pVmMgr = NULL;
#endif

//#define  CONNTYPE    VIX_SERVICEPROVIDER_VMWARE_WORKSTATION
#define  CONNTYPE    VIX_SERVICEPROVIDER_VMWARE_PLAYER

#define  HOSTNAME ""
#define  HOSTPORT 0
#define  USERNAME ""
#define  PASSWORD ""

#define  VMPOWEROPTIONS   VIX_VMPOWEROP_LAUNCH_GUI   // Launches the VMware Workstaion UI
// when powering on the virtual machine.

#define VMXPATH_INFO "where vmxpath is an absolute path to the .vmx file " \
                     "for the virtual machine."

HWND lv_hwndFound = NULL;
BOOL CALLBACK EnumWindowsProcVM(HWND hwnd, LPARAM lParam) {
    wchar_t windowTitle[256];

    // Get the window title
    GetWindowText(hwnd, windowTitle, sizeof(windowTitle));
    _wcsupr_s(windowTitle, 256);

    if (wcsstr(windowTitle, L"VMWARE WORKSTATION") != NULL
    && wcsstr(windowTitle, L"PLAYER") != NULL) {
        lv_hwndFound = hwnd;
        return FALSE;
    }

    return TRUE; // Continue enumeration
}

typedef BOOL
(WINAPI* FN_CreateProcessW)(
    _In_opt_ LPCWSTR lpApplicationName,
    _Inout_opt_ LPWSTR lpCommandLine,
    _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ BOOL bInheritHandles,
    _In_ DWORD dwCreationFlags,
    _In_opt_ LPVOID lpEnvironment,
    _In_opt_ LPCWSTR lpCurrentDirectory,
    _In_ LPSTARTUPINFOW lpStartupInfo,
    _Out_ LPPROCESS_INFORMATION lpProcessInformation
    );
FN_CreateProcessW lv_orgCreateProcessW;

typedef BOOL
(WINAPI* FN_CreateProcessA)(
    _In_opt_ LPCSTR lpApplicationName,
    _Inout_opt_ LPSTR lpCommandLine,
    _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ BOOL bInheritHandles,
    _In_ DWORD dwCreationFlags,
    _In_opt_ LPVOID lpEnvironment,
    _In_opt_ LPCSTR lpCurrentDirectory,
    _In_ LPSTARTUPINFOA lpStartupInfo,
    _Out_ LPPROCESS_INFORMATION lpProcessInformation
    );
FN_CreateProcessA lv_orgCreateProcessA;

BOOL WINAPI hook_CreateProcessW(
    _In_opt_ LPCWSTR lpApplicationName,
    _Inout_opt_ LPWSTR lpCommandLine,
    _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ BOOL bInheritHandles,
    _In_ DWORD dwCreationFlags,
    _In_opt_ LPVOID lpEnvironment,
    _In_opt_ LPCWSTR lpCurrentDirectory,
    _In_ LPSTARTUPINFOW lpStartupInfo,
    _Out_ LPPROCESS_INFORMATION lpProcessInformation
)
{
    BOOL bRet = lv_orgCreateProcessW(
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation
    );

    if (lpApplicationName == NULL || wcsstr(lpApplicationName, L"vmplayer.exe") == NULL) {
        return bRet;
    }

    if (bRet) {
        wchar_t wszDllPath[MAX_PATH]; memset(wszDllPath, 0, sizeof(wszDllPath));
        swprintf_s(wszDllPath, MAX_PATH, L"%s\\KenanHelperVM32.dll", ClaPathMgr::GetDP());
        ClaProcess::InjectDll(lpProcessInformation->hProcess, wszDllPath);
//        ResumeThread(lpProcessInformation->hThread);
    }
    return bRet;
}

BOOL WINAPI hook_CreateProcessA(
    _In_opt_ LPCSTR lpApplicationName,
    _Inout_opt_ LPSTR lpCommandLine,
    _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ BOOL bInheritHandles,
    _In_ DWORD dwCreationFlags,
    _In_opt_ LPVOID lpEnvironment,
    _In_opt_ LPCSTR lpCurrentDirectory,
    _In_ LPSTARTUPINFOA lpStartupInfo,
    _Out_ LPPROCESS_INFORMATION lpProcessInformation
)
{
    BOOL bRet = lv_orgCreateProcessA(
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation
    );
    if (lpApplicationName == NULL || strstr(lpApplicationName, "vmplayer.exe") == NULL) {
        return bRet;
    }

    if (bRet) {
        wchar_t wszDllPath[MAX_PATH]; memset(wszDllPath, 0, sizeof(wszDllPath));
        swprintf_s(wszDllPath, MAX_PATH, L"%s\\KenanHelperVM32.dll", ClaPathMgr::GetDP());
        ClaProcess::InjectDll(lpProcessInformation->hProcess, wszDllPath);
        //ResumeThread(lpProcessInformation->hThread);
    }
    return bRet;
}

HANDLE lv_mutexVm;
typedef enum _ENUM_VM_PROCESS {
    vm_process_unknown = 0,
    vm_process_start = 1,
    vm_process_stop,
    vm_process_suspend,
}ENUM_VM_PROCESS;
CString lv_strVMX;
CString lv_strPwd;
CString lv_strEncPwd;
ENUM_VM_PROCESS lv_nVmProc;

#ifndef WIN64
UINT TF_VM_PROCESS(void* p) {
    lv_pVmMgr->_vm_process();
    return 0;
}
#endif

HMODULE lv_hModHelper = NULL;
CString lv_strDecryptVmx;

void KVmMgr::_vm_process(void)
{
#ifndef WIN64
    lv_nVmProc = vm_process_unknown;

    while (lv_thdVmProgress != NULL) {

        DWORD dwWait = WaitForSingleObject(lv_mutexVm, 2000);
        if (lv_nVmProc == vm_process_unknown || dwWait == WAIT_TIMEOUT) {
            //if (!lv_strDecryptVmx.IsEmpty() && lv_nVmProc == vm_process_unknown)
            //    DeleteFile(lv_strDecryptVmx);
            continue;
        }

        if (lv_nVmProc == vm_process_start) {
            lv_hModHelper = LoadLibrary(L"KenanHelperVM32.dll");

            int nRet = _launchVM(lv_strVMX, lv_strPwd);
            Sleep(1000);
            DeleteFile(lv_strDecryptVmx);
            PostMessage(_hwndMain, WM_USER_VM_LAUNCHED, nRet, 0);
            lv_nVmProc = vm_process_unknown;
            continue;
        }

        if (lv_nVmProc == vm_process_stop) {
            int nRet = _stopVM();
            if (lv_hModHelper) {
                FreeLibrary(lv_hModHelper);
                lv_hModHelper = NULL;
            }

            {
                wchar_t wszPwd[128];
                if (!lv_strVMX.IsEmpty() && GetFileAttributes(lv_strVMX) != INVALID_FILE_ATTRIBUTES) {
                    if (kn_encrypt_file_with_pwd(lv_strDecryptVmx, lv_strVMX, lv_strEncPwd, lv_strPwd) != 0) {
                    }
                }
                DeleteFile(lv_strDecryptVmx);
            }

            PostMessage(_hwndMain, WM_USER_VM_STOPED, nRet, 0);
            lv_nVmProc = vm_process_unknown;
            continue;
        }

        if (lv_nVmProc == vm_process_suspend) {
            int nRet = _suspendVM();
            if (lv_hModHelper) {
                FreeLibrary(lv_hModHelper);
                lv_hModHelper = NULL;
            }
            {
                wchar_t wszPwd[128];
                if (!lv_strVMX.IsEmpty() && GetFileAttributes(lv_strVMX) != INVALID_FILE_ATTRIBUTES) {
                    if (kn_encrypt_file_with_pwd(lv_strDecryptVmx, lv_strVMX, lv_strEncPwd, lv_strPwd) != 0) {
                    }
                }
                DeleteFile(lv_strDecryptVmx);
            }

            PostMessage(_hwndMain, WM_USER_VM_SUSPENDED, nRet, 0);
            lv_nVmProc = vm_process_unknown;
            continue;
        }
    }
#endif
    return;
}

void KVmMgr::initialize(HWND p_hwndMain)
{
    CString strPath, strDir, strCmd;
    strPath.Format(L"%s\\launcher64.exe", ClaPathMgr::GetDP());
    strDir = ClaPathMgr::GetDP();
    strCmd.Format(L"--pdnd");
    ClaProcess::CreateProcessEx(strPath, strCmd, strDir, NULL, NULL);

    //. hook createprocess function to hook when the vmplayer is run.
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    HMODULE hModKernel = LoadLibraryW(L"kernelbase.dll");
    if (hModKernel)
    {
        lv_orgCreateProcessA = (FN_CreateProcessA)GetProcAddress(hModKernel, "CreateProcessA");
        lv_orgCreateProcessW = (FN_CreateProcessW)GetProcAddress(hModKernel, "CreateProcessW");
    }

    if (DetourAttach(&(PVOID&)lv_orgCreateProcessA, hook_CreateProcessA) != NO_ERROR) {
        OutputDebugStringW(L"err");
    }
    if (DetourAttach(&(PVOID&)lv_orgCreateProcessW, hook_CreateProcessW) != NO_ERROR) {
        OutputDebugStringW(L"err");
    }

    if (DetourTransactionCommit() != NO_ERROR) {
        OutputDebugStringW(L"err");
    }

#ifndef WIN64
    DWORD dwTID;
    lv_pVmMgr = this;
    _hwndMain = p_hwndMain;
    lv_mutexVm = CreateEvent(NULL, FALSE, FALSE, L"VM_PROCESS");
    lv_thdVmProgress = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_VM_PROCESS, NULL, 0, &dwTID);
#endif
}

int KVmMgr::encryptVMX(const wchar_t* p_wszPath, const char* p_szOld, const char* p_szNew)
{
    wchar_t wszOld[MAX_PATH]; memset(wszOld, 0, sizeof(wszOld));
    wchar_t wszNew[MAX_PATH]; memset(wszNew, 0, sizeof(wszNew));

    swprintf_s(wszOld, MAX_PATH, L"%S", p_szOld);
    swprintf_s(wszNew, MAX_PATH, L"%S", p_szNew);

    if (p_szOld[0] == 0 || kn_is_encrypt_file(p_wszPath) == FALSE) {
        //. input no password, in this case, vmx must not be encrypted.
        if (kn_is_encrypt_file(p_wszPath) == TRUE) {
            return kn_reencrypt_file_with_pwd(p_wszPath, p_wszPath, L"Kenan-vm-encrypt-1226", wszNew, L"");
        }

        kn_encrypt_file_with_pwd(p_wszPath, p_wszPath, wszNew, L"");
        return 0;
    }
    else {
        if (kn_reencrypt_file_with_pwd(p_wszPath, p_wszPath, wszOld, wszNew, L"") != 0) {
            return -1;
        }
        return 0;
    }
    return 0;
}

int KVmMgr::launchVM(const wchar_t* p_wszPath, const char* p_szPwd, const char* p_szEncPwd)
{
    wchar_t wszEncPwd[MAX_PATH]; 
    swprintf_s(wszEncPwd, MAX_PATH, L"%S", p_szEncPwd);
    wchar_t wszPwd[MAX_PATH];
    swprintf_s(wszPwd, MAX_PATH, L"%S", p_szPwd);

    lv_nVmProc = vm_process_start;
    //
    //. Decrypt file
    //
    {

        lv_strDecryptVmx = p_wszPath;
        lv_strDecryptVmx.Replace(L".vmx", L".vml");

        ClaRegMgr reg(HKEY_LOCAL_MACHINE);
        reg.writeStringW(L"Software\\kenan", L"vmx", lv_strVMX);
        reg.writeStringW(L"Software\\kenan", L"dvmx", lv_strDecryptVmx);

        if (kn_decrypt_file_with_pwd(p_wszPath, lv_strDecryptVmx, wszEncPwd, NULL, 0) != 0) {
            lv_nVmProc = vm_process_unknown;
            return -1;
        }
        lv_strEncPwd = wszEncPwd;

        lv_strVMX = p_wszPath;
        lv_strPwd = wszPwd;
    }
    SetEvent(lv_mutexVm);
    return 0;
}

int KVmMgr::_launchVM(const wchar_t* p_wszPath, const wchar_t* p_wszPwd)
{
    int nRet = 0;
#ifndef WIN64
    if (lv_handleVM != NULL) {
        Vix_ReleaseHandle(lv_handleVM);
        lv_handleVM = NULL;
    }

    char szPath[MAX_PATH]; memset(szPath, 0, sizeof(szPath));
    sprintf_s(szPath, "%S", p_wszPath);
    char szPassword[MAX_PATH]; memset(szPassword, 0, sizeof(szPassword));
    sprintf_s(szPassword, "%S", p_wszPwd);

    VixError err;
    char* vmxPath;
    VixHandle hostHandle = VIX_INVALID_HANDLE;
    VixHandle jobHandle = VIX_INVALID_HANDLE;
    VixHandle vmHandle = VIX_INVALID_HANDLE;
    VixHandle propertyHandle = VIX_INVALID_HANDLE;
    VixHandle snapshotHandle = VIX_INVALID_HANDLE;

    jobHandle = VixHost_Connect(VIX_API_VERSION,
        CONNTYPE,
        HOSTNAME, // *hostName,
        HOSTPORT, // hostPort,
        USERNAME, // *userName,
        PASSWORD, // *password,
        0, // options,
        VIX_INVALID_HANDLE, // propertyListHandle,
        NULL, // *callbackProc,
        NULL); // *clientData);
    err = VixJob_Wait(jobHandle,
        VIX_PROPERTY_JOB_RESULT_HANDLE,
        &hostHandle,
        VIX_PROPERTY_NONE);
    if (VIX_FAILED(err)) {
        nRet = -1;
        goto abort;
    }

    err = VixPropertyList_AllocPropertyList(hostHandle,
        &propertyHandle,
        VIX_PROPERTY_VM_ENCRYPTION_PASSWORD,
        szPassword,
        VIX_PROPERTY_NONE);
    if (err != VIX_OK) {
        nRet = -2;
        goto abort;
    }

    lv_propertyHandle = propertyHandle;


    Vix_ReleaseHandle(jobHandle);
    jobHandle = VixHost_OpenVM(hostHandle,
        szPath,
        VIX_VMOPEN_NORMAL,
        propertyHandle,
        NULL, // VixEventProc *callbackProc,
        NULL); // void *clientData);
    err = VixJob_Wait(jobHandle,
        VIX_PROPERTY_JOB_RESULT_HANDLE,
        &vmHandle,
        VIX_PROPERTY_NONE);
    if (VIX_FAILED(err)) {
        if (err == 17005) {
            SetLastError(ERROR_PASSWORD_RESTRICTION);
        }
        else {
            SetLastError(err);
        }
        nRet = -3;
        goto abort;
    }

    lv_handleVM = vmHandle;

    // Stop replaying the virtual machine.
    jobHandle = VixVM_Unpause(vmHandle,
        0, // options
        propertyHandle, // propertyListHandle
        NULL, // callbackProc
        NULL); // clientData
    err = VixJob_Wait(jobHandle,
        VIX_PROPERTY_NONE);


    Vix_ReleaseHandle(jobHandle);
    jobHandle = VixVM_PowerOn(vmHandle,
        VMPOWEROPTIONS,
        propertyHandle,
        NULL, // *callbackProc,
        NULL); // *clientData);
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    if (VIX_FAILED(err)) {
        nRet = -4;
        goto abort;
    }

    // Stop replaying the virtual machine.
    jobHandle = VixVM_Unpause(vmHandle,
        0, // options
        propertyHandle, // propertyListHandle
        NULL, // callbackProc
        NULL); // clientData
    err = VixJob_Wait(jobHandle,
        VIX_PROPERTY_NONE);

    m_strVMX = p_wszPath;
    m_strPWD = p_wszPwd;

abort:
#endif
    return nRet;
}

int KVmMgr::stopVM()
{
    lv_nVmProc = vm_process_stop;
    SetEvent(lv_mutexVm);
    return 0;
}

int KVmMgr::_stopVM()
{
    int nRet = 0;
#ifndef WIN64
    char szPath[MAX_PATH]; memset(szPath, 0, sizeof(szPath));
    sprintf_s(szPath, "%S", m_strVMX.GetBuffer());
    char szPassword[MAX_PATH]; memset(szPassword, 0, sizeof(szPassword));
    sprintf_s(szPassword, "%S", m_strPWD.GetBuffer());

    VixError err;
    char* vmxPath;
    VixHandle hostHandle = VIX_INVALID_HANDLE;
    VixHandle jobHandle = VIX_INVALID_HANDLE;
    VixHandle vmHandle = VIX_INVALID_HANDLE;
    VixHandle propertyHandle = VIX_INVALID_HANDLE;
    VixHandle snapshotHandle = VIX_INVALID_HANDLE;

    //jobHandle = VixHost_Connect(VIX_API_VERSION,
    //    CONNTYPE,
    //    HOSTNAME, // *hostName,
    //    HOSTPORT, // hostPort,
    //    USERNAME, // *userName,
    //    PASSWORD, // *password,
    //    0, // options,
    //    VIX_INVALID_HANDLE, // propertyListHandle,
    //    NULL, // *callbackProc,
    //    NULL); // *clientData);
    //err = VixJob_Wait(jobHandle,
    //    VIX_PROPERTY_JOB_RESULT_HANDLE,
    //    &hostHandle,
    //    VIX_PROPERTY_NONE);
    //if (VIX_FAILED(err)) {
    //    nRet = -1;
    //    goto abort;
    //}

    //Vix_ReleaseHandle(jobHandle);
    //err = VixPropertyList_AllocPropertyList(hostHandle,
    //    &propertyHandle,
    //    VIX_PROPERTY_VM_ENCRYPTION_PASSWORD,
    //    szPassword,
    //    VIX_PROPERTY_NONE);
    //if (err != VIX_OK) {
    //    nRet = -2;
    //    goto abort;
    //}

    //jobHandle = VixHost_OpenVM(hostHandle,
    //    szPath,
    //    VIX_VMOPEN_NORMAL,
    //    propertyHandle,
    //    NULL, // VixEventProc *callbackProc,
    //    NULL); // void *clientData);
    //err = VixJob_Wait(jobHandle,
    //    VIX_PROPERTY_JOB_RESULT_HANDLE,
    //    &vmHandle,
    //    VIX_PROPERTY_NONE);
    //if (VIX_FAILED(err)) {
    //    nRet = -3;
    //    goto abort;
    //}

    //Vix_ReleaseHandle(jobHandle);
    if (lv_handleVM != NULL)
    {
        jobHandle = VixVM_PowerOff(lv_handleVM,
            0,
            NULL, NULL); // *clientData);
        err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
        if (VIX_FAILED(err)) {
            nRet = -4;
            goto abort;
        }
    }

abort:
    Vix_ReleaseHandle(jobHandle);
    Vix_ReleaseHandle(propertyHandle);
    Vix_ReleaseHandle(vmHandle);
    Vix_ReleaseHandle(hostHandle);

    if (lv_handleVM != NULL) {
        Vix_ReleaseHandle(lv_handleVM);
        lv_handleVM = NULL;
    }
    lv_propertyHandle = NULL;
#endif
    return 0;
}

int KVmMgr::suspendVM() {
    lv_nVmProc = vm_process_suspend;
    SetEvent(lv_mutexVm);
    return 0;
}

int KVmMgr::_suspendVM()
{
    int nRet = 0;
#ifndef WIN64
    char szPath[MAX_PATH]; memset(szPath, 0, sizeof(szPath));
    sprintf_s(szPath, "%S", m_strVMX.GetBuffer());
    char szPassword[MAX_PATH]; memset(szPassword, 0, sizeof(szPassword));
    sprintf_s(szPassword, "%S", m_strPWD.GetBuffer());

    VixError err;
    char* vmxPath;
    VixHandle hostHandle = VIX_INVALID_HANDLE;
    VixHandle jobHandle = VIX_INVALID_HANDLE;
    VixHandle vmHandle = VIX_INVALID_HANDLE;
    VixHandle propertyHandle = VIX_INVALID_HANDLE;
    VixHandle snapshotHandle = VIX_INVALID_HANDLE;

    //jobHandle = VixVM_Pause(lv_handleVM,
    //    0, // options
    //    lv_propertyHandle, // propertyListHandle
    //    NULL, // callbackProc
    //    NULL); // clientData
    //err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    //if (VIX_FAILED(err)) {
    //    nRet = -4;
    //    goto abort;
    //}
    if (lv_handleVM != NULL)
    {
        jobHandle = VixVM_Suspend(lv_handleVM,
            0, // options
            NULL, // callbackProc
            NULL); // clientData
        err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
        if (VIX_FAILED(err)) {
            nRet = -4;
            goto abort;
        }
    }

abort:
    Vix_ReleaseHandle(jobHandle);
    Vix_ReleaseHandle(propertyHandle);
    Vix_ReleaseHandle(vmHandle);
    Vix_ReleaseHandle(hostHandle);

    if (lv_handleVM != NULL) {
        Vix_ReleaseHandle(lv_handleVM);
        lv_handleVM = NULL;
    }
    lv_propertyHandle = NULL;

#endif
    return 0;
}

int KVmMgr::getIP(char* p_szIP, int size)
{
    int nRet = 0;
#ifndef WIN64
    
    VixError err;
    char* vmxPath;
    VixHandle hostHandle = VIX_INVALID_HANDLE;
    VixHandle jobHandle = VIX_INVALID_HANDLE;
    VixHandle vmHandle = VIX_INVALID_HANDLE;
    VixHandle propertyHandle = VIX_INVALID_HANDLE;
    VixHandle snapshotHandle = VIX_INVALID_HANDLE;
    char* pIP = NULL;

    if (lv_handleVM == NULL) {
        return -1;
    }

    /*
    jobHandle = VixHost_Connect(VIX_API_VERSION,
        CONNTYPE,
        HOSTNAME, // *hostName,
        HOSTPORT, // hostPort,
        USERNAME, // *userName,
        PASSWORD, // *password,
        0, // options,
        VIX_INVALID_HANDLE, // propertyListHandle,
        NULL, // *callbackProc,
        NULL); // *clientData);
    err = VixJob_Wait(jobHandle,
        VIX_PROPERTY_JOB_RESULT_HANDLE,
        &hostHandle,
        VIX_PROPERTY_NONE);
    if (VIX_FAILED(err)) {
        nRet = -1;
        goto abort;
    }

    Vix_ReleaseHandle(jobHandle);
    err = VixPropertyList_AllocPropertyList(hostHandle,
        &propertyHandle,
        VIX_PROPERTY_VM_ENCRYPTION_PASSWORD,
        szPassword,
        VIX_PROPERTY_NONE);
    if (err != VIX_OK) {
        nRet = -2;
        goto abort;
    }


    jobHandle = VixHost_OpenVM(hostHandle,
        szPath,
        VIX_VMOPEN_NORMAL,
        propertyHandle,
        NULL, // VixEventProc *callbackProc,
        NULL); // void *clientData);
    err = VixJob_Wait(jobHandle,
        VIX_PROPERTY_JOB_RESULT_HANDLE,
        &vmHandle,
        VIX_PROPERTY_NONE);
    if (VIX_FAILED(err)) {
        nRet = -3;
        goto abort;
    }
    */
    Vix_ReleaseHandle(jobHandle);
    jobHandle = VixVM_ReadVariable(lv_handleVM,
        VIX_VM_GUEST_VARIABLE,
        "ip",
        0, // options
        NULL, // callbackProc
        NULL); // clientData);
    err = VixJob_Wait(jobHandle,
        VIX_PROPERTY_JOB_RESULT_VM_VARIABLE_STRING,
        &pIP,
        VIX_PROPERTY_NONE);
    if (VIX_OK != err) {
        // Handle the error...
        goto abort;
    }

    if (p_szIP != NULL) {
        strcpy_s(p_szIP, size, pIP);
    }

abort:
    /*
    Vix_ReleaseHandle(jobHandle);
    Vix_ReleaseHandle(propertyHandle);
    Vix_ReleaseHandle(vmHandle);
    Vix_ReleaseHandle(hostHandle);
    */
#endif
    return 0;
}

BOOL KVmMgr::IsRunning(const wchar_t* p_wszPath, const wchar_t* p_wszPwd)
{
    BOOL bRet = FALSE;
#ifndef WIN64
    VixHandle hostHandle = VIX_INVALID_HANDLE;
    VixHandle jobHandle = VIX_INVALID_HANDLE;
    VixHandle vmHandle = VIX_INVALID_HANDLE;
    VixHandle propertyHandle = VIX_INVALID_HANDLE;
    VixError err;
    char szPassword[200]; memset(szPassword, 0, sizeof(szPassword));
    sprintf_s(szPassword, 200, "%S", p_wszPwd);
    char szPath[200]; memset(szPath, 0, sizeof(szPath));
    sprintf_s(szPath, 200, "%S", p_wszPath);
    int vmPowerState;
    /*
    jobHandle = VixHost_Connect(VIX_API_VERSION,
        CONNTYPE,
        HOSTNAME, // *hostName,
        HOSTPORT, // hostPort,
        USERNAME, // *userName,
        PASSWORD, // *password,
        0, // options,
        VIX_INVALID_HANDLE, // propertyListHandle,
        NULL, // *callbackProc,
        NULL); // *clientData);
    err = VixJob_Wait(jobHandle,
        VIX_PROPERTY_JOB_RESULT_HANDLE,
        &hostHandle,
        VIX_PROPERTY_NONE);
    if (VIX_FAILED(err)) {
        goto L_EXIT;
    }

    err = VixPropertyList_AllocPropertyList(hostHandle,
        &propertyHandle,
        VIX_PROPERTY_VM_ENCRYPTION_PASSWORD,
        szPassword,
        VIX_PROPERTY_NONE);
    if (err != VIX_OK) {
        goto L_EXIT;
    }


    Vix_ReleaseHandle(jobHandle);
    jobHandle = VixHost_OpenVM(hostHandle,
        szPath,
        VIX_VMOPEN_NORMAL,
        propertyHandle,
        NULL, // VixEventProc *callbackProc,
        NULL); // void *clientData);
    err = VixJob_Wait(jobHandle,
        VIX_PROPERTY_JOB_RESULT_HANDLE,
        &vmHandle,
        VIX_PROPERTY_NONE);
    if (VIX_FAILED(err)) {
        goto L_EXIT;
    }
    */
    if (lv_handleVM == NULL) 
        return FALSE;
    err = Vix_GetProperties(lv_handleVM,
        VIX_PROPERTY_VM_POWER_STATE,
        &vmPowerState,
        VIX_PROPERTY_NONE);
    if (VIX_OK != err) {
        goto L_EXIT;
    }

    bRet = (vmPowerState & VIX_POWERSTATE_POWERED_ON) != 0;
L_EXIT:
#endif
    return bRet;
}

BOOL KVmMgr::isWorking() {
    return lv_nVmProc != vm_process_unknown;
}