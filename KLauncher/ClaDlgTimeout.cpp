// ClaDlgTimeout.cpp : implementation file
//

#include "pch.h"
#include "klauncher.h"
#include "afxdialogex.h"
#include "ClaDlgTimeout.h"


// ClaDlgTimeout dialog

ClaDlgTimeout::ClaDlgTimeout(UINT p_nBgID, CWnd* pParent /*=nullptr*/)
	: KDialog(IDD_DLG_TIMEOUT, p_nBgID, L"PNG", NULL, pParent, TRUE)
	, m_strMessage(_T(""))
	, m_strMessage1(_T("Do you want to extend?"))
{
	m_bIDOK = TRUE;
}

ClaDlgTimeout::~ClaDlgTimeout()
{
}

void ClaDlgTimeout::DoDataExchange(CDataExchange* pDX)
{
	KDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STC_MESSAGE, m_strMessage);
	DDX_Control(pDX, IDOK, m_alert_btn_ok);
	DDX_Control(pDX, IDCANCEL, m_alert_btn_cancel);
	DDX_Control(pDX, IDC_BTN_EXIT, m_btnClose);
	DDX_Control(pDX, IDC_STC_MESSAGE, m_strDescription);
	DDX_Control(pDX, IDC_STC_MESSAGE1, m_stcMessage1);
	DDX_Text(pDX, IDC_STC_MESSAGE1, m_strMessage1);
}


BEGIN_MESSAGE_MAP(ClaDlgTimeout, KDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &ClaDlgTimeout::OnBnClickedOk)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_EXIT, &ClaDlgTimeout::OnBnClickedBtnExit)
END_MESSAGE_MAP()


// ClaDlgTimeout message handlers


BOOL ClaDlgTimeout::OnInitDialog()
{
	KDialog::OnInitDialog();
//	ModifyStyle(0, WS_CAPTION);
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

	KDialog::OnTimer(nIDEvent);
}


void ClaDlgTimeout::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}


void ClaDlgTimeout::OnDestroy()
{
	KillTimer(0);

	KDialog::OnDestroy();
}
KDialog* ClaDlgTimeout::CreateStandardDialog()
{
	return NULL;
}
void ClaDlgTimeout::OnInitChildrenWnds()
{

	m_strDescription.setFont(L"Arial", 12, TRUE, RGB(255, 255, 255));
	m_strDescription.setAlign(1);
	RegisterTranslucentWnd(&m_strDescription);

	m_stcMessage1.setFont(L"Arial", 11, TRUE, RGB(255, 255, 255));
	m_stcMessage1.setAlign(0);
	RegisterTranslucentWnd(&m_stcMessage1);

	UINT nImgIdCloses[TWS_BUTTON_NUM] = { IDB_PNG_BTN_CLOSE, IDB_PNG_BTN_CLOSE_H, 0, 0 };
	m_btnClose.LoadImageList(nImgIdCloses);

	RegisterTranslucentWnd(&m_btnClose);

	UINT nImgIdRed[TWS_BUTTON_NUM] = { IDB_PNG_BTN_SMALL_RED, IDB_PNG_BTN_SMALL_RED_H, IDB_PNG_BTN_SMALL_RED, 0, IDB_PNG_BTN_SMALL_D };
	UINT nImgIdBlue[TWS_BUTTON_NUM] = { IDB_PNG_BTN_SMALL_BLUE, IDB_PNG_BTN_SMALL_BLUE_H, IDB_PNG_BTN_SMALL_BLUE, 0, IDB_PNG_BTN_SMALL_D };

	m_alert_btn_ok.LoadImageList(nImgIdBlue, L"PNG", NULL, FALSE);
	m_alert_btn_ok.setFont(L"Arial", 11, FALSE, RGB(0xFF, 0xFF, 0xFF));
	m_alert_btn_ok.setAlign(1);
	RegisterTranslucentWnd(&m_alert_btn_ok);

	m_alert_btn_cancel.LoadImageList(nImgIdRed, L"PNG", NULL, FALSE);
	m_alert_btn_cancel.setFont(L"Arial", 11, FALSE, RGB(0xFF, 0xFF, 0xFF));
	m_alert_btn_cancel.setAlign(1);
	RegisterTranslucentWnd(&m_alert_btn_cancel);
}

void ClaDlgTimeout::OnBnClickedBtnExit()
{
	CDialog::OnCancel();
	// TODO: Add your control notification handler code here
}
