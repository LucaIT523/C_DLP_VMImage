// ClaDlgBound.cpp : implementation file
//

#include "pch.h"
#include "KenanHelperVM.h"
#include "afxdialogex.h"

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

#include "ClaDlgBound.h"

extern HWND lv_hwndMainWindow;

// ClaDlgBound dialog
extern HINSTANCE g_hInstance;

ClaDlgBound::ClaDlgBound(CWnd* pParent /*=nullptr*/)
	: KDialog(IDD_DLG_BOUND, IDB_PNG_BORDER, L"PNG", g_hInstance, pParent, TRUE)
{

}

ClaDlgBound::~ClaDlgBound()
{
}

void ClaDlgBound::DoDataExchange(CDataExchange* pDX)
{
	KDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STC_ICON, m_stcIcon);
}


BEGIN_MESSAGE_MAP(ClaDlgBound, KDialog)
	ON_WM_ACTIVATE()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// ClaDlgBound message handlers


void ClaDlgBound::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	KDialog::OnActivate(nState, pWndOther, bMinimized);

	SetTimer(0, 10, NULL);
}


void ClaDlgBound::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 0) {
		::SetForegroundWindow(lv_hwndMainWindow);
		KillTimer(0);
	}

	KDialog::OnTimer(nIDEvent);
}

void ClaDlgBound::OnInitChildrenWnds()
{
	m_stcIcon.LoadImageK(IDB_PNG_TOPBAR);
	RegisterTranslucentWnd(&m_stcIcon);
}


void ClaDlgBound::OnSize(UINT nType, int cx, int cy)
{
	KDialog::OnSize(nType, cx, cy);

	if (m_stcIcon.m_hWnd)
	{
		CRect rtMain;
		GetClientRect(rtMain);

		CRect rt;
		m_stcIcon.GetClientRect(rt);

		CRect rtVM;
		GetWindowRect(rtVM);
		ScreenToClient(rtVM);

		CRect rtNew;
		rtNew.left = (rtMain.Width() - rt.Width()) / 2;
		rtNew.right = rtNew.left + rt.Width();
		rtNew.top = rtVM.top+1;
		rtNew.bottom = rtNew.top + rt.Height();

		m_stcIcon.MoveWindow(rtNew);
	}
}


LRESULT ClaDlgBound::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_USER + 1) {
		RECT* pRect = (RECT*)wParam;
		MoveWindow(pRect);
		ShowWindow(SW_SHOW);
	}

	return KDialog::WindowProc(message, wParam, lParam);
}


BOOL ClaDlgBound::OnInitDialog()
{
	KDialog::OnInitDialog();

	LONG_PTR style = GetWindowLongPtr(m_hWnd, GWL_EXSTYLE);
	style |= WS_EX_TOOLWINDOW; // Add the tool window style
	style &= ~WS_EX_APPWINDOW; // Remove the app window style
	SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, style);

	::SetForegroundWindow(lv_hwndMainWindow);

	SetTimer(1, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

std::vector<HWND> hwndList;

BOOL CALLBACK EnumWindowsProcNext(HWND hwnd, LPARAM lParam) {
	hwndList.push_back(hwnd);
	return TRUE; // Continue enumeration
}

HWND GetNextHwnd(HWND currentHwnd) {
	hwndList.clear(); // Clear previous HWNDs
	EnumWindows(EnumWindowsProcNext, 0); // Enumerate all top-level windows

	for (size_t i = 0; i < hwndList.size(); ++i) {
		if (hwndList[i] == currentHwnd) {
			// Return the next HWND in the list
			if (i + 1 < hwndList.size()) {
				return hwndList[i - 1];
			}
			else {
				return nullptr; // No next HWND
			}
		}
	}
	return nullptr; // Current HWND not found
}

BOOL lv_bTopmost = TRUE;
void ClaDlgBound::setTopmost(BOOL p_bTopmost)
{
	if (lv_bTopmost == FALSE && p_bTopmost) {
		lv_bTopmost = TRUE;
		::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		OutputDebugString(L"SfT - topmost\n");
	}
	if (lv_bTopmost && !p_bTopmost){
		lv_bTopmost = FALSE;

		::SetWindowPos(m_hWnd, GetNextHwnd(lv_hwndMainWindow), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		//::WindowFromPoint()
		OutputDebugString(L"SfT - nontopmost\n");
	}
}
