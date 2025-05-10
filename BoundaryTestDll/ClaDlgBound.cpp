// ClaDlgBound.cpp : implementation file
//

#include "pch.h"
#include "BoundaryTestDll.h"
#include "afxdialogex.h"

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

#include "ClaDlgBound.h"

extern HWND lv_hwndMainWindow;

// ClaDlgBound dialog
extern HINSTANCE g_hInstance;

ClaDlgBound::ClaDlgBound(CWnd* pParent /*=nullptr*/)
	: KDialog(IDD_DLG_BOUND, IDB_PNG1, L"PNG", g_hInstance, pParent, TRUE)
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

		CRect rtNew;
		rtNew.left = (rtMain.Width() - rt.Width()) / 2;
		rtNew.right = rtNew.left + rt.Width();
		rtNew.top = rt.top;
		rtNew.bottom = rt.bottom;

		m_stcIcon.MoveWindow(rtNew);
	}
}


BOOL ClaDlgBound::OnInitDialog()
{
	KDialog::OnInitDialog();

	LONG_PTR style = GetWindowLongPtr(m_hWnd, GWL_EXSTYLE);
	style |= WS_EX_TOOLWINDOW; // Add the tool window style
	style &= ~WS_EX_APPWINDOW; // Remove the app window style
	SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, style);

	// TODO:  Add extra initialization here
	::SetForegroundWindow(lv_hwndMainWindow);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
