// ClaDlgAlert.cpp : implementation file
//

#include "pch.h"
#include "KLauncher.h"
#include "afxdialogex.h"
#include "ClaDlgAlert.h"
#include "framework.h"
#include "KGlobal.h"

#define WM_USER_CONN_VALIDATE	(WM_USER+0x1001)
// ClaDlgAlert dialog

ClaDlgAlert::ClaDlgAlert(UINT p_nBgID, CWnd* pParent /*=nullptr*/)
	: KDialog(IDD_DLG_ALERT, p_nBgID, L"PNG", NULL, pParent, TRUE)
{
	m_alertText = L"unknown Error";
	m_imageID = IDB_PNG_ICON_DANGER;
	m_isCancel = TRUE;
	m_isSuspend = FALSE;
	m_isStartContinueMode = FALSE;
	_msgID = 0;
}
ClaDlgAlert::~ClaDlgAlert()
{
}

void ClaDlgAlert::DoDataExchange(CDataExchange* pDX)
{
	KDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STAATIC_ICON, m_alert_icon);
	DDX_Control(pDX, IDC_STATIC_DESCRIPTION, m_alert_description);
	DDX_Control(pDX, IDCANCEL, m_alert_btn_cancel);
	DDX_Control(pDX, IDOK, m_alert_btn_ok);
	DDX_Control(pDX, IDOK2, m_alert_btn_ok2);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BTN_ICON, m_alert_icon_btn);
	DDX_Control(pDX, IDC_BTN_SUSPEND3, m_btnSuspend);
	DDX_Control(pDX, IDC_BTN_STOP3, m_btnStop3);
	DDX_Control(pDX, IDC_BTN_CANCEL3, m_btnCancel3);
}


BEGIN_MESSAGE_MAP(ClaDlgAlert, KDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &ClaDlgAlert::OnBnClickedOk)
	ON_BN_CLICKED(IDOK2, &ClaDlgAlert::OnBnClickedOk2)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &ClaDlgAlert::OnBnClickedBtnClose)
	ON_BN_CLICKED(IDCANCEL, &ClaDlgAlert::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_CANCEL3, &ClaDlgAlert::OnBnClickedBtnCancel3)
	ON_BN_CLICKED(IDC_BTN_SUSPEND3, &ClaDlgAlert::OnBnClickedBtnSuspend3)
	ON_BN_CLICKED(IDC_BTN_STOP3, &ClaDlgAlert::OnBnClickedBtnStop3)
END_MESSAGE_MAP()

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void ClaDlgAlert::OnPaint()	
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		KDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR ClaDlgAlert::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

KDialog* ClaDlgAlert::CreateStandardDialog()
{
	return NULL;
}

void ClaDlgAlert::OnInitChildrenWnds()
{
	UINT nImgIdCloses[TWS_BUTTON_NUM] = { IDB_PNG_BTN_CLOSE, IDB_PNG_BTN_CLOSE_H, 0, 0 };
	m_btnClose.LoadImageList(nImgIdCloses);

	RegisterTranslucentWnd(&m_btnClose);

	m_alert_description.setFont(L"Arial", 12, TRUE, RGB(255, 255, 255));
	m_alert_description.setAlign(1);
	m_alert_description.SetWindowTextW(m_alertText);
	RegisterTranslucentWnd(&m_alert_description);

	UINT nImgIdRed[TWS_BUTTON_NUM] = { IDB_PNG_BTN_SMALL_RED, IDB_PNG_BTN_SMALL_RED_H, IDB_PNG_BTN_SMALL_RED, 0, IDB_PNG_BTN_SMALL_D};
	UINT nImgIdBlue[TWS_BUTTON_NUM] = { IDB_PNG_BTN_SMALL_BLUE, IDB_PNG_BTN_SMALL_BLUE_H, IDB_PNG_BTN_SMALL_BLUE, 0, IDB_PNG_BTN_SMALL_D };

	m_alert_btn_ok2.LoadImageList(nImgIdBlue, L"PNG", NULL, FALSE);
	m_alert_btn_ok2.setFont(L"Arial", 11, FALSE, RGB(0xFF, 0xFF, 0xFF));
	m_alert_btn_ok2.setAlign(1);
	RegisterTranslucentWnd(&m_alert_btn_ok2);

	m_alert_btn_ok.LoadImageList(nImgIdBlue, L"PNG", NULL, FALSE);
	m_alert_btn_ok.setFont(L"Arial", 11, FALSE, RGB(0xFF, 0xFF, 0xFF));
	m_alert_btn_ok.setAlign(1);
	RegisterTranslucentWnd(&m_alert_btn_ok);

	m_alert_btn_cancel.LoadImageList(nImgIdRed, L"PNG", NULL, FALSE);
	m_alert_btn_cancel.setFont(L"Arial", 11, FALSE, RGB(0xFF, 0xFF, 0xFF));
	m_alert_btn_cancel.setAlign(1);
	RegisterTranslucentWnd(&m_alert_btn_cancel);

	m_btnSuspend.LoadImageList(nImgIdRed, L"PNG", NULL, FALSE);
	m_btnSuspend.setFont(L"Arial", 11, FALSE, RGB(0xFF, 0xFF, 0xFF));
	m_btnSuspend.setAlign(1);
	RegisterTranslucentWnd(&m_btnSuspend);
	m_btnStop3.LoadImageList(nImgIdRed, L"PNG", NULL, FALSE);
	m_btnStop3.setFont(L"Arial", 11, FALSE, RGB(0xFF, 0xFF, 0xFF));
	m_btnStop3.setAlign(1);
	RegisterTranslucentWnd(&m_btnStop3);
	m_btnCancel3.LoadImageList(nImgIdRed, L"PNG", NULL, FALSE);
	m_btnCancel3.setFont(L"Arial", 11, FALSE, RGB(0xFF, 0xFF, 0xFF));
	m_btnCancel3.setAlign(1);
	RegisterTranslucentWnd(&m_btnCancel3);

	m_alert_icon.LoadImageK(m_imageID);
	UINT nImgIdIcon[TWS_BUTTON_NUM] = { m_imageID, m_imageID, m_imageID, 0 };
	m_alert_icon_btn.LoadImageList(nImgIdIcon);
	RegisterTranslucentWnd(&m_alert_icon_btn);
	if (m_isSuspend) {
		m_alert_btn_ok2.ShowWindow(SW_HIDE);
		m_alert_btn_ok.ShowWindow(SW_HIDE);
		m_alert_btn_cancel.ShowWindow(SW_HIDE);
		m_btnSuspend.ShowWindow(SW_SHOW);
		m_btnStop3.ShowWindow(SW_SHOW);
		m_btnCancel3.ShowWindow(SW_SHOW);
	}
	else if (m_isStartContinueMode) {
		m_alert_btn_ok2.ShowWindow(SW_HIDE);
		m_alert_btn_ok.ShowWindow(SW_HIDE);
		m_alert_btn_cancel.ShowWindow(SW_HIDE);

		m_btnSuspend.SetWindowTextW(L"Continue");
		m_btnSuspend.ShowWindow(SW_SHOW);
		m_btnStop3.SetWindowTextW(L"Start");
		m_btnStop3.ShowWindow(SW_SHOW);
		m_btnCancel3.ShowWindow(SW_SHOW);
	}
	else {
		if (!m_isCancel) {
			m_alert_btn_ok2.ShowWindow(SW_SHOW);
			m_alert_btn_ok.ShowWindow(SW_HIDE);
			m_alert_btn_cancel.ShowWindow(SW_HIDE);
		}
		else {
			m_alert_btn_ok2.ShowWindow(SW_HIDE);
			m_alert_btn_ok.ShowWindow(SW_SHOW);
			m_alert_btn_cancel.ShowWindow(SW_SHOW);
		}
		m_btnSuspend.ShowWindow(SW_HIDE);
		m_btnStop3.ShowWindow(SW_HIDE);
		m_btnCancel3.ShowWindow(SW_HIDE);
	}
}



void ClaDlgAlert::setAlertText(LPCTSTR p_alertText){
	m_alertText = p_alertText;
	//m_alert_description.SetWindowTextW(p_alertText);
}
void ClaDlgAlert::setAlertIcon(UINT p_imageID){
	m_imageID = p_imageID;
	//UINT nImgIdIcon[TWS_BUTTON_NUM] = { p_imageID, p_imageID, p_imageID, 0 };
	//m_alert_icon_btn.LoadImageList(nImgIdIcon);
}
void ClaDlgAlert::setCancelMode(BOOL p_isCancel) {
	m_isCancel = p_isCancel;
}
// ClaDlgAlert message handlers
void ClaDlgAlert::setSuspendMode(BOOL p_isSuspend) {
	m_isSuspend = p_isSuspend;
}
void ClaDlgAlert::setStartContinueMode(BOOL p_bMode) {
	m_isStartContinueMode = p_bMode;
}
// ClaDlgAlert message handlers


void ClaDlgAlert::OnBnClickedOk()
{
	CDialog::OnOK();
	// TODO: Add your control notification handler code here
}


void ClaDlgAlert::OnBnClickedOk2()
{
	CDialog::OnOK();
	// TODO: Add your control notification handler code here
}


void ClaDlgAlert::OnBnClickedBtnClose()
{
	CDialog::OnCancel();
}


void ClaDlgAlert::OnBnClickedCancel()
{
	CDialog::OnCancel();
	// TODO: Add your control notification handler code here
}


void ClaDlgAlert::OnBnClickedBtnCancel3()
{
	CDialog::OnCancel();
	// TODO: Add your control notification handler code here
}


void ClaDlgAlert::OnBnClickedBtnSuspend3()
{
	EndDialog(IDTRYAGAIN);
}


void ClaDlgAlert::OnBnClickedBtnStop3()
{
	CDialog::OnOK();
	// TODO: Add your control notification handler code here
}
