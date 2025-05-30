// ClaPageOTP.cpp : implementation file
//

#include "pch.h"
#include "KLauncher.h"
#include "KLauncherDlg.h"
#include "afxdialogex.h"
#include "ClaPageOTP.h"
#include "KC_common.h"

// ClaPageOTP dialog

ClaPageOTP::ClaPageOTP(CWnd* pParent /*=nullptr*/)
	: KDialog(IDD_PAGE_OTP, 0, L"PNG", 0, pParent, FALSE)
	, m_strPin1(_T(""))
{

}

ClaPageOTP::~ClaPageOTP()
{
}

void ClaPageOTP::DoDataExchange(CDataExchange* pDX)
{
	KDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STC_LOGO, m_stcLogo);
	DDX_Control(pDX, IDC_STC_TITLE, m_stcTitle);
	DDX_Control(pDX, IDC_STC_TITLE2, m_stcTitle2);
	DDX_Control(pDX, IDC_STC_TITLE3, m_stcTitle3);
	DDX_Control(pDX, IDC_STC_PANEL, m_stcPanel);

	DDX_Control(pDX, IDC_EDT_PIN1, m_edtPin1);
	DDX_Control(pDX, IDC_EDT_PIN2, m_edtPin2);
	DDX_Control(pDX, IDC_EDT_PIN3, m_edtPin3);
	DDX_Control(pDX, IDC_EDT_PIN4, m_edtPin4);
	DDX_Control(pDX, IDC_EDT_PIN5, m_edtPin5);
	DDX_Control(pDX, IDC_EDT_PIN6, m_edtPin6);

	DDX_Control(pDX, IDC_STC_RESEND, m_stcResend);
	DDX_Control(pDX, IDCANCEL, m_btnBack);
	DDX_Control(pDX, IDC_BTN_LOGIN, m_btnLogin);

	DDX_Text(pDX, IDC_EDT_PIN1, m_strPin1);
	DDX_Text(pDX, IDC_EDT_PIN2, m_strPin2);
	DDX_Text(pDX, IDC_EDT_PIN3, m_strPin3);
	DDX_Text(pDX, IDC_EDT_PIN4, m_strPin4);
	DDX_Text(pDX, IDC_EDT_PIN5, m_strPin5);
	DDX_Text(pDX, IDC_EDT_PIN6, m_strPin6);
}

BEGIN_MESSAGE_MAP(ClaPageOTP, KDialog)
	ON_BN_CLICKED(IDCANCEL, &ClaPageOTP::OnBnClickedBtnBack)

	ON_EN_CHANGE(IDC_EDT_PIN1, &ClaPageOTP::OnEnChangeEdtPin1)
	ON_EN_CHANGE(IDC_EDT_PIN2, &ClaPageOTP::OnEnChangeEdtPin2)
	ON_EN_CHANGE(IDC_EDT_PIN3, &ClaPageOTP::OnEnChangeEdtPin3)
	ON_EN_CHANGE(IDC_EDT_PIN4, &ClaPageOTP::OnEnChangeEdtPin4)
	ON_EN_CHANGE(IDC_EDT_PIN5, &ClaPageOTP::OnEnChangeEdtPin5)
	ON_EN_CHANGE(IDC_EDT_PIN6, &ClaPageOTP::OnEnChangeEdtPin6)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &ClaPageOTP::OnBnClickedBtnLogin)
END_MESSAGE_MAP()


// ClaPageOTP message handlers


BOOL ClaPageOTP::OnInitDialog()
{
	KDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void ClaPageOTP::OnInitChildrenWnds() {
	m_stcLogo.LoadImageK(IDB_PNG_LOGO);
	RegisterTranslucentWnd(&m_stcLogo);

	m_stcTitle.setFont(L"Arial", 24, TRUE, RGB(255, 255, 255));
	m_stcTitle.setAlign(1);
	RegisterTranslucentWnd(&m_stcTitle);

	m_stcTitle2.setFont(L"Arial", 9, FALSE, RGB(255, 255, 255));
	m_stcTitle2.setAlign(1);
	RegisterTranslucentWnd(&m_stcTitle2);

	m_stcTitle3.setFont(L"Arial", 9, FALSE, RGB(255, 255, 255));
	m_stcTitle3.setAlign(1);
	RegisterTranslucentWnd(&m_stcTitle3);

	m_stcPanel.LoadImageK(IDB_PNG_PANEL_OTP);
	RegisterTranslucentWnd(&m_stcPanel);

	UINT nImgIdPin[4] = { IDB_PNG_PIN_UNSEL, IDB_PNG_PIN_SEL, IDB_PNG_PIN_FILL_UNSEL, IDB_PNG_PIN_FILL_SEL };

	m_edtPin1.LoadImageK(nImgIdPin);
	m_edtPin2.LoadImageK(nImgIdPin);
	m_edtPin3.LoadImageK(nImgIdPin);
	m_edtPin4.LoadImageK(nImgIdPin);
	m_edtPin5.LoadImageK(nImgIdPin);
	m_edtPin6.LoadImageK(nImgIdPin);

	RegisterTranslucentWnd(&m_edtPin1);
	RegisterTranslucentWnd(&m_edtPin2);
	RegisterTranslucentWnd(&m_edtPin3);
	RegisterTranslucentWnd(&m_edtPin4);
	RegisterTranslucentWnd(&m_edtPin5);
	RegisterTranslucentWnd(&m_edtPin6);

	m_stcResend.setFont(L"Arial", 10, TRUE, RGB(0x3F, 0x6F, 0xFF));
	m_stcResend.setAlign(1);
	RegisterTranslucentWnd(&m_stcTitle3);

	UINT nImgIdBack[TWS_BUTTON_NUM] = { IDB_PNG_BTN_HALF_R, IDB_PNG_BTN_HALF_RH, IDB_PNG_BTN_HALF_RH, 0 };
	m_btnBack.LoadImageList(nImgIdBack);
	m_btnBack.setFont(L"Arial", 12, FALSE, RGB(0xFF, 0xFF, 0xFF));
	m_btnBack.setAlign(1);
	RegisterTranslucentWnd(&m_btnBack);

	UINT nImgIdLogin[TWS_BUTTON_NUM] = { IDB_PNG_BTN_HALF_B, IDB_PNG_BTN_HALF_BH, IDB_PNG_BTN_HALF_BH, 0 };
	m_btnLogin.LoadImageList(nImgIdLogin);
	m_btnLogin.setFont(L"Arial", 12, FALSE, RGB(0xFF, 0xFF, 0xFF));
	m_btnLogin.setAlign(1);
	RegisterTranslucentWnd(&m_btnLogin);

	UpdateView();
}

void ClaPageOTP::OnBnClickedBtnBack()
{
	g_pMain->ShowLoginPage();
}

void ClaPageOTP::OnEnChangeEdtPin1()
{
	UpdateData(TRUE);
	if (!m_strPin1.IsEmpty()) {
		int nCuror = ((CEdit*)&m_edtPin1)->GetSel();
		m_strPin1 = m_strPin1.Mid(nCuror - 1, 1);
		m_edtPin2.SetFocus();
		((CEdit*)&m_edtPin2)->SetSel(0, -1);
		((CEdit*)&m_edtPin1)->SetSel(0, -1);
	}
}

void ClaPageOTP::OnEnChangeEdtPin2()
{
	UpdateData(TRUE);
	if (!m_strPin2.IsEmpty()) {
		int nCuror = ((CEdit*)&m_edtPin2)->GetSel();
		m_strPin2 = m_strPin2.Mid(nCuror - 1, 1);
		m_edtPin3.SetFocus();
		((CEdit*)&m_edtPin3)->SetSel(0, -1);
		((CEdit*)&m_edtPin2)->SetSel(0, -1);
	}
	else {
		m_edtPin1.SetFocus();
		((CEdit*)&m_edtPin1)->SetSel(0, -1);
	}
}

void ClaPageOTP::OnEnChangeEdtPin3()
{
	UpdateData(TRUE);
	if (!m_strPin3.IsEmpty()) {
		int nCuror = ((CEdit*)&m_edtPin3)->GetSel();
		m_strPin3 = m_strPin3.Mid(nCuror - 1, 1);
		m_edtPin4.SetFocus();
		((CEdit*)&m_edtPin4)->SetSel(0, -1);
		((CEdit*)&m_edtPin3)->SetSel(0, -1);
	}
	else {
		m_edtPin2.SetFocus();
		((CEdit*)&m_edtPin2)->SetSel(0, -1);
	}
}

void ClaPageOTP::OnEnChangeEdtPin4()
{
	UpdateData(TRUE);
	if (!m_strPin4.IsEmpty()) {
		int nCuror = ((CEdit*)&m_edtPin4)->GetSel();
		m_strPin4 = m_strPin4.Mid(nCuror - 1, 1);
		m_edtPin5.SetFocus();
		((CEdit*)&m_edtPin5)->SetSel(0, -1);
		((CEdit*)&m_edtPin4)->SetSel(0, -1);
	}
	else {
		m_edtPin3.SetFocus();
		((CEdit*)&m_edtPin3)->SetSel(0, -1);
	}
}

void ClaPageOTP::OnEnChangeEdtPin5()
{
	UpdateData(TRUE);
	if (!m_strPin5.IsEmpty()) {
		int nCuror = ((CEdit*)&m_edtPin5)->GetSel();
		m_strPin5 = m_strPin5.Mid(nCuror - 1, 1);
		m_edtPin6.SetFocus();
		((CEdit*)&m_edtPin6)->SetSel(0, -1);
		((CEdit*)&m_edtPin5)->SetSel(0, -1);
	}
	else {
		m_edtPin4.SetFocus();
		((CEdit*)&m_edtPin4)->SetSel(0, -1);
	}
}

void ClaPageOTP::OnEnChangeEdtPin6()
{
	UpdateData(TRUE);
	if (!m_strPin6.IsEmpty()) {
		int nCuror = ((CEdit*)&m_edtPin6)->GetSel();
		m_strPin6 = m_strPin6.Mid(nCuror - 1, 1);
	}
	else {
		m_edtPin5.SetFocus();
		((CEdit*)&m_edtPin5)->SetSel(0, -1);
	}
}

int lv_nCountOtp = 3;

void ClaPageOTP::OnBnClickedBtnLogin()
{
	UpdateData(TRUE);

	CString opCode = m_strPin1 + m_strPin2+ m_strPin3 + m_strPin4 + m_strPin5 + m_strPin6;

	char szOpCode[10]; memset(szOpCode, 0, sizeof(szOpCode));
	sprintf_s(szOpCode, 10, "%S", (LPCWSTR)opCode);

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - opt conn");
	int nSts = kc_otp(g_strMachineID, opCode, g_strGlobalIP, CALLBACK_VALIDATE, g_pMain);

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - opt =%d[%d]", nSts, GetLastError());

	if (nSts != 0)
	{
		if (kc_get_err() == ERROR_SERVER_DISABLED) {
			KMessageBox(L"Unable to connect to the server, please check your internet connectivity or retry again later");
			return;
		}
		else {
			KMessageBox(L"Please input the correct OTP code.");
			m_strPin1 = m_strPin2 = m_strPin3 = m_strPin4 = m_strPin5 = m_strPin6 = L"";
			m_edtPin1.SetFocus();
			UpdateData(FALSE);
			if (lv_nCountOtp > 1) {
				lv_nCountOtp--;
				return;
			}
			else {
				g_pMain->OnCancel();
				return;
			}
		}
	}

	char szID[10]; kc_get_uid(szID);
	char szSession[64]; kc_get_session(szSession, 64);

	g_strUserID.Format(L"%S", szID);
	g_strSessionID.Format(L"%S", szSession);

	theApp.relaunchClient(FALSE);

	g_pMain->ShowMainPage();
}

void ClaPageOTP::OnShowPage()
{
	m_strPin1 = L"";
	m_strPin2 = L"";
	m_strPin3 = L"";
	m_strPin4 = L"";
	m_strPin5 = L"";
	m_strPin6 = L"";

	m_edtPin1.SetFocus();

	UpdateData(FALSE);
}