#include "pch.h"
#include "KenanHelperVM.h"
#include "ClaDlgBound.h"
#include "KenanBoundary.h"
#include <gdiplus.h>
#include <vector>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

ClaDlgBound* dlg;
HINSTANCE lv_hInstance;
HANDLE lv_thdHookMessage = NULL;
HHOOK lv_hookWndProc = NULL;
HWND lv_hwndMainWindow = NULL;
DWORD lv_dwMainThreadID;
UINT TF_HookMessage(void*);
HWND FindMainWindow();
LRESULT CALLBACK HookWndProc(int nCode, WPARAM wParam, LPARAM lParam);
BOOL lv_bCreated = FALSE;
void StartCaptureWinProc();

#define LD_SPACE 8

void startDrawBoundary(HINSTANCE p_hInstance) {
    GdiplusStartupInput m_gdiplusStartupInput;
    ULONG_PTR m_gdiplusToken;
    GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, nullptr);

    lv_hInstance = p_hInstance;

    StartCaptureWinProc();
}

void stopDrawBoundary()
{
    if (lv_hookWndProc)
        lv_hookWndProc = SetWindowsHookEx(WH_CALLWNDPROC, HookWndProc, lv_hInstance, lv_dwMainThreadID);
    lv_hookWndProc = NULL;
    lv_thdHookMessage = NULL;
    Sleep(1000);

    if (lv_thdHookMessage) {
        TerminateThread(lv_thdHookMessage, 0);
    }
}

void StartCaptureWinProc()
{
    DWORD dwTID;
    lv_thdHookMessage = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_HookMessage, NULL, 0, &dwTID);
}

void MyMoveBoundary() {
    RECT rt;
    GetWindowRect(lv_hwndMainWindow, &rt);

    rt.left -= LD_SPACE;
    rt.top -= LD_SPACE;
    rt.right += LD_SPACE;
    rt.bottom += LD_SPACE;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    if (rt.left < 2) rt.left = 1;
    if (rt.top < 2) rt.top = 1;
    if (rt.right > screenWidth - 2) rt.right = screenWidth - 1;
    if (rt.bottom > screenHeight - 2) rt.bottom = screenHeight - 1;

    dlg->MoveWindow(rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top);
    dlg->ShowWindow(TRUE);
}

UINT TF_HookMessage(void*)
{
    dlg = new ClaDlgBound();
    dlg->Create(IDD_DLG_BOUND);

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
    lv_hookWndProc = SetWindowsHookEx(WH_CALLWNDPROC, HookWndProc, lv_hInstance, lv_dwMainThreadID);
    MyMoveBoundary();

    BOOL bShow = FALSE;
    MSG msg;
    while (lv_thdHookMessage && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    dlg->DestroyWindow();
    delete dlg;
    dlg = NULL;
    return 0;
}

BOOL CALLBACK EnumWindowsProcForBoundary(HWND hwnd, LPARAM lParam)
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
        wcsstr(wszWindowName, L"VMware") != NULL &&
        wcsstr(wszWindowName, L"Player") != NULL)
    {
        lv_dwMainThreadID = dwTID;
        HWND* pHwndRet = (HWND*)lParam;
        if (pHwndRet != NULL) *pHwndRet = hwnd;
        return FALSE;
    }

    return TRUE;

}

HWND FindMainWindow()
{
    HWND hwndRet = NULL;
    // Enumerate all top-level windows
    EnumWindows(EnumWindowsProcForBoundary, (LPARAM)&hwndRet);

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


    // Check if the message is for the target window
    if (pCwp->hwnd == lv_hwndMainWindow)
    {
        switch (pCwp->message)
        {
        case WM_SIZE:
            MyMoveBoundary();
            break;
        case WM_MOVE:
            MyMoveBoundary();
            break;
        case WM_ACTIVATEAPP:
        case WM_ACTIVATE:
        case WM_WINDOWPOSCHANGING:
        case WM_WINDOWPOSCHANGED:
            dlg->setTopmost(GetForegroundWindow() == lv_hwndMainWindow);
            break;
        }
    }
    // Call the next hook in the chain
    int nRet = CallNextHookEx(lv_hookWndProc, nCode, wParam, lParam);
    return nRet;
}