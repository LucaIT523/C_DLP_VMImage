// ClaDlgTimeout.cpp : implementation file
//

#include "pch.h"
#include "launcher.h"
#include "afxdialogex.h"
#include "ClaDlgTimeout.h"


// ClaDlgTimeout dialog

IMPLEMENT_DYNAMIC(ClaDlgTimeout, CDialog)

ClaDlgTimeout::ClaDlgTimeout(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_TIMEOUT, pParent)
	, m_strMessage(_T(""))
{
	m_bIDOK = TRUE;
}

ClaDlgTimeout::~ClaDlgTimeout()
{
}

void ClaDlgTimeout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STC_MESSAGE, m_strMessage);
}


BEGIN_MESSAGE_MAP(ClaDlgTimeout, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &ClaDlgTimeout::OnBnClickedOk)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// ClaDlgTimeout message handlers


BOOL ClaDlgTimeout::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_strMessage = m_strData;
	UpdateData(FALSE);

	m_nSecond = 0;
	SetWindowText(m_strTitle);

	SetTimer(0, 1000, NULL);

	if (m_bIDOK == FALSE) {
		GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDCANCEL)->SetWindowTextW(L"Close");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void ClaDlgTimeout::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 0) {
		if (m_nSecond > m_nMax) {
			OnCancel();
		}
		else {
			m_strMessage.Format(L"%s\n%d second(s) left.", m_strData, m_nMax - m_nSecond);
			m_nSecond++;
			UpdateData(FALSE);
		}
	}

	CDialog::OnTimer(nIDEvent);
}


void ClaDlgTimeout::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}


void ClaDlgTimeout::OnDestroy()
{
	KillTimer(0);

	CDialog::OnDestroy();
}
