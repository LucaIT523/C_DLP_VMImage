#include "pch.h"
#include "ClaOsInfo.h"
#include <Windows.h>
#include <VersionHelpers.h>
#include "ClaRegMgr.h"

#include <comdef.h>
#include <Wbemidl.h>
#include <string>
#include <winsvc.h>

#pragma comment(lib, "wbemuuid.lib")

#pragma comment(lib, "ntdll.lib")

// Define NTSTATUS and related types if not included
typedef LONG NTSTATUS;
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

// Function prototype for RtlGetVersion
extern "C" NTSTATUS NTAPI RtlGetVersion(POSVERSIONINFOEX lpVersionInformation);


BOOL ClaOSInfo::is64bitWindows()
{
#if defined(_WIN64)
	return TRUE;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
	// 32-bit programs run on both 32-bit and 64-bit Windows
	// so must sniff
	BOOL f64 = FALSE;
	return IsWow64Process(GetCurrentProcess(), &f64) && f64;
#else
	return FALSE; // Win64 does not support Win16
#endif
}

std::string ClaOSInfo::getOS()
{
    std::string osVersion;

    // Prepare the OSVERSIONINFOEX structure
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    // Get the OS version using RtlGetVersion
    NTSTATUS status = RtlGetVersion(&osvi);
    if (status != STATUS_SUCCESS) {
        return "Unable to determine OS version";
    }

    // Determine the version of Windows based on major and minor version numbers
    if (osvi.dwMajorVersion == 10) {
        if (osvi.dwMinorVersion == 0) {
            if (osvi.dwBuildNumber >= 22000) {
                osVersion = "Win11";
            }
            else {
                osVersion = "Win10";
            }
        }
    }
    else if (osvi.dwMajorVersion == 6) {
        if (osvi.dwMinorVersion == 3) {
            osVersion = "Win8.1";
        }
        else if (osvi.dwMinorVersion == 2) {
            osVersion = "Win8";
        }
        else if (osvi.dwMinorVersion == 1) {
            osVersion = "Win7";
        }
        else if (osvi.dwMinorVersion == 0) {
            osVersion = "WinVista";
        }
    }
    else if (osvi.dwMajorVersion == 5) {
        if (osvi.dwMinorVersion == 2) {
            osVersion = "WinServer2003";
        }
        else if (osvi.dwMinorVersion == 1) {
            osVersion = "WinXP";
        }
    }
    else {
        osVersion = "Unknown Windows version";
    }

    return osVersion;
}

int ClaOSInfo::getCpuInfo(int* p_nCoreCnt, int* p_nProcessors, int *p_nSpeed)
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    DWORD numLogicalProcessors = sysInfo.dwNumberOfProcessors;

    // Get core count
    DWORD numCores = 0;
    DWORD bufferSize = 0;
    GetLogicalProcessorInformation(NULL, &bufferSize);
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION* buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)malloc(bufferSize);
    GetLogicalProcessorInformation(buffer, &bufferSize);

    for (DWORD i = 0; i < bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
        if (buffer[i].Relationship == RelationProcessorCore) {
            numCores++;
        }
    }
    free(buffer);

    ClaRegMgr reg(HKEY_LOCAL_MACHINE);
    DWORD dwSpeed;
    reg.readInt(L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", L"~MHz", dwSpeed);

    if (p_nCoreCnt != NULL) *p_nCoreCnt = numCores;
    if (p_nProcessors != NULL) *p_nProcessors = numLogicalProcessors;
    if (p_nSpeed != NULL) *p_nSpeed = dwSpeed;

    return 0;
}

int ClaOSInfo::getRamInfo(unsigned long long* p_nTotal)
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    if (p_nTotal) *p_nTotal = memInfo.ullTotalPhys;
    return 0;
}

int ClaOSInfo::getDriveSize(const wchar_t* p_wszDrv, unsigned long long* p_lTotal, unsigned long long* p_lFree)
{
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
    if (GetDiskFreeSpaceEx(p_wszDrv, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
        if (p_lTotal) *p_lTotal = totalNumberOfBytes.QuadPart;
        if (p_lFree) *p_lFree = freeBytesAvailable.QuadPart;
        return 0;
    }
    return -1;
}

bool isDefenderActive() {
    // Check if the Windows Defender service is running
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (hSCManager) {
        SC_HANDLE hService = OpenService(hSCManager, L"WinDefend", SERVICE_QUERY_STATUS);
        if (hService) {
            SERVICE_STATUS ss;
            if (QueryServiceStatus(hService, &ss)) {
                CloseServiceHandle(hService);
                CloseServiceHandle(hSCManager);
                return ss.dwCurrentState == SERVICE_RUNNING;
            }
            CloseServiceHandle(hService);
        }
        CloseServiceHandle(hSCManager);
    }

    return false;
}

int ClaOSInfo::getAntivirusList(std::wstring& l)
{
    HRESULT hres;

    // Initialize COM
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres)) {
        return -1;
    }

    // Initialize security
    hres = CoInitializeSecurity(
        NULL,
        -1,                          // COM negotiates service
        NULL,                        // Authentication
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,  // Default authentication
        RPC_C_IMP_LEVEL_IMPERSONATE, // Impersonation
        NULL,                        // Reserved
        EOAC_NONE,                  // Additional capabilities
        NULL                         // Reserved
    );

    // Initialize WMI
    IWbemLocator* pLoc = NULL;
    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLoc);

    IWbemServices* pSvc = NULL;
    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"),
        NULL,
        NULL,
        0,
        NULL,
        0,
        0,
        &pSvc);

    // Set security levels on the proxy
    hres = CoInitializeSecurity(
        NULL,
        -1,                          // COM negotiates service
        NULL,                        // Authentication
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,  // Default authentication
        RPC_C_IMP_LEVEL_IMPERSONATE, // Impersonation
        NULL,                        // Reserved
        EOAC_NONE,                  // Additional capabilities
        NULL                         // Reserved
    );

    // Query for installed antivirus products
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM AntiVirusProduct"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    // Get the data from the query
    IWbemClassObject* pclsObj = NULL;
    ULONG uReturn = 0;

    bool first = true; // To manage the separator
    while (pEnumerator) {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (0 == uReturn) {
            break;
        }

        VARIANT vtName;
        VariantInit(&vtName);
        hr = pclsObj->Get(L"displayName", 0, &vtName, 0, 0);
        if (SUCCEEDED(hr) && vtName.vt == VT_BSTR) {
            if (!first) {
                l += L"|"; // Add separator
            }
            l += vtName.bstrVal; // Append antivirus name
            first = false; // After the first entry, we have more
        }
        VariantClear(&vtName);
        pclsObj->Release();
    }

    // Cleanup
    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();

    // Check for Microsoft Defender
    if (isDefenderActive()) {
        if (!first) {
            l += L"|"; // Add separator if there are other products
        }
        l += L"Microsoft Defender"; // Append Defender name
    }
}