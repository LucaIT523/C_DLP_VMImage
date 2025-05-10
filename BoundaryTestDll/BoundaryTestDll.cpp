// BoundaryTestDll.cpp : Defines the initialization routines for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "BoundaryTestDll.h"
#include "ClaDlgBound.h"

#include <gdiplus.h>
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CBoundaryTestDllApp
BEGIN_MESSAGE_MAP(CBoundaryTestDllApp, CWinApp)
END_MESSAGE_MAP()

// CBoundaryTestDllApp construction
CBoundaryTestDllApp::CBoundaryTestDllApp(){}

// The one and only CBoundaryTestDllApp object
CBoundaryTestDllApp theApp;
ClaDlgBound  *dlg;
HINSTANCE g_hInstance;
void StartCaptureWinProc();
HANDLE lv_thdHookMessage;
HHOOK lv_hookWndProc;
HWND lv_hwndMainWindow = NULL;
DWORD lv_dwMainThreadID;
UINT TF_HookMessage(void*);
HWND FindMainWindow();
LRESULT CALLBACK HookWndProc(int nCode, WPARAM wParam, LPARAM lParam);
BOOL lv_bCreated = FALSE;

// CBoundaryTestDllApp initialization
BOOL CBoundaryTestDllApp::InitInstance()
{
	CWinApp::InitInstance();
    GdiplusStartupInput m_gdiplusStartupInput;
    ULONG_PTR m_gdiplusToken;
    GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, nullptr);

    g_hInstance = m_hInstance;

    StartCaptureWinProc();

	return TRUE;
}

int CBoundaryTestDllApp::ExitInstance()
{
    delete dlg;

    return CWinApp::ExitInstance();
}

void StartCaptureWinProc()
{
    DWORD dwTID;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_HookMessage, NULL, 0, &dwTID);
}

UINT TF_HookMessage(void*)
{
    dlg = new ClaDlgBound();
    dlg->Create(IDD_DLG_BOUND);
    //dlg->ShowWindow(SW_SHOW);

    while (TRUE) {
        lv_hwndMainWindow = FindMainWindow();
        if (lv_hwndMainWindow != NULL) {
            break;
        }
        Sleep(1000);
    }

    wchar_t wszTitle[64];
    GetWindowText(lv_hwndMainWindow, wszTitle, 64);
    lv_dwMainThreadID = GetWindowThreadProcessId(lv_hwndMainWindow, NULL);
    lv_hookWndProc = SetWindowsHookEx(WH_CALLWNDPROC, HookWndProc, g_hInstance, lv_dwMainThreadID);

    RECT rt;
    GetWindowRect(lv_hwndMainWindow, &rt);
    dlg->MoveWindow(rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top);
    dlg->ShowWindow(TRUE);

    BOOL bShow = FALSE;
    MSG msg;
    while (lv_hookWndProc != NULL && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    DWORD dwPID, dwTID;
    
    dwTID = GetWindowThreadProcessId(hwnd, &dwPID);
    if (GetCurrentProcessId() != dwPID)
        return TRUE;

    wchar_t wszClassName[MAX_PATH];
    wchar_t wszWindowName[MAX_PATH];
    GetClassName(hwnd, wszClassName, MAX_PATH);
    GetWindowTextW(hwnd, wszWindowName, MAX_PATH);
    if (wcscmp(wszClassName, L"MSCTFIME UI") == 0) {
        return TRUE;
    }
    if (wcscmp(wszClassName, L"IME") == 0) {
        return TRUE;
    }
    if (wcscmp(wszClassName, L"#32770") == 0 && wcscmp(wszWindowName, L"") == 0) {
        return TRUE;
    }

    HWND hwndTemp = GetParent(hwnd);
    if (hwndTemp != NULL)
        return TRUE;

    if (wcscmp(wszClassName, L"VMPlayerFrame") == 0 &&
        wcscmp(wszWindowName, L"VMware Workstation 17 Player") == 0)
    {

    }


    lv_dwMainThreadID = dwTID;
    HWND* pHwndRet = (HWND*)lParam;
    if (pHwndRet != NULL) *pHwndRet = hwnd;
    return FALSE;
}

HWND FindMainWindow()
{
    HWND hwndRet = NULL;
    // Enumerate all top-level windows
    EnumWindows(EnumWindowsProc, (LPARAM) &hwndRet);

    return hwndRet;
}

std::vector<HWND> getAllWindows() {
    std::vector<HWND> windows;

    // EnumWindows will call this function for each top-level window
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        std::vector<HWND>* pWindows = reinterpret_cast<std::vector<HWND>*>(lParam);
        if (IsWindowVisible(hwnd) && GetWindow(hwnd, GW_OWNER) == NULL) {
            pWindows->push_back(hwnd);
        }
        return TRUE; // Continue enumeration
        }, reinterpret_cast<LPARAM>(&windows));

    return windows;
}

HWND getNextWindow(HWND current) {
    std::vector<HWND> windows = getAllWindows();
    for (size_t i = 0; i < windows.size(); ++i) {
        if (windows[i] == current) {
            // Return the next window, wrapping around to the first if needed
            return windows[(i + 1) % windows.size()];
        }
    }
    return NULL; // Current window not found
}

LRESULT CALLBACK HookWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    CWPSTRUCT* pCwp = reinterpret_cast<CWPSTRUCT*>(lParam);

    RECT rt;
    int left, top, width, height;
    HWND hwndOld;
    LONG_PTR style;

    int nRet = CallNextHookEx(lv_hookWndProc, nCode, wParam, lParam);

    // Check if the message is for the target window
    //if (pCwp->hwnd == lv_hwndMainWindow)
    //{
        switch (pCwp->message)
        {
        case WM_SIZE:
            OutputDebugString(_T("WM_SIZE message intercepted.\n"));
            GetWindowRect(lv_hwndMainWindow, &rt);
            dlg->MoveWindow(rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top);
            dlg->ShowWindow(TRUE);
            break;
        case WM_MOVE:
            OutputDebugString(_T("WM_MOVE message intercepted.\n"));
            GetWindowRect(lv_hwndMainWindow, &rt);
            dlg->MoveWindow(rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top);
            dlg->ShowWindow(TRUE);
            break;
        case WM_ACTIVATEAPP:
            //if (GetForegroundWindow() == lv_hwndMainWindow) {
                ::SetWindowPos(lv_hwndMainWindow, dlg->m_hWnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            //}
            //else {
                //::SetWindowPos(dlg->m_hWnd, lv_hwndMainWindow, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            //}
            break;
        }
    //}
    // Call the next hook in the chain
    return nRet;
}