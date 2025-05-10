// ClaDlgOtp.cpp : implementation file
//

#include "pch.h"
#include "launcher.h"
#include "afxdialogex.h"
#include "ClaDlgOtp.h"
#include "ini.h"
#include "ClaPathMgr.h"

#include "ClaProcess.h"
#include "SessionManager.h"
#include "KC_common.h"

// ClaDlgOtp dialog

IMPLEMENT_DYNAMIC(ClaDlgOtp, ClaDialogPng)

ClaDlgOtp::ClaDlgOtp(CWnd* pParent /*=nullptr*/)
	: ClaDialogPng(IDD_DLG_OTP, pParent)
	, m_strNum1(_T(""))
	, m_strNum2(_T(""))
	, m_strNum3(_T(""))
	, m_strNum4(_T(""))
	, m_strNum5(_T(""))
	, m_strNum6(_T(""))
{
	m_bBack = FALSE;
}

ClaDlgOtp::~ClaDlgOtp()
{
}

void ClaDlgOtp::DoDataExchange(CDataExchange* pDX)
{
	ClaDialogPng::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_NUM1, m_strNum1);
	DDX_Text(pDX, IDC_EDT_NUM2, m_strNum2);
	DDX_Text(pDX, IDC_EDT_NUM3, m_strNum3);
	DDX_Text(pDX, IDC_EDT_NUM4, m_strNum4);
	DDX_Text(pDX, IDC_EDT_NUM5, m_strNum5);
	DDX_Text(pDX, IDC_EDT_NUM6, m_strNum6);
	DDX_Control(pDX, IDC_STC_GRP_BG, m_grpBG);
	DDX_Control(pDX, IDC_STC_ICON, m_stcLogo);
	DDX_Control(pDX, IDC_GRP_OTP, m_grpOTP);
	DDX_Control(pDX, IDC_STC_OTP, m_stcOTP);
	DDX_Control(pDX, IDC_STC_RESEND, m_stcResend);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_STC_TITLE, m_stcTitle);
	DDX_Control(pDX, IDC_STC_SUBTITLE, m_stcSubTitle);
	DDX_Control(pDX, IDC_EDT_NUM1, m_edtNum1);
	DDX_Control(pDX, IDC_EDT_NUM2, m_edtNum2);
	DDX_Control(pDX, IDC_EDT_NUM3, m_edtNum3);
	DDX_Control(pDX, IDC_EDT_NUM4, m_edtNum4);
	DDX_Control(pDX, IDC_EDT_NUM5, m_edtNum5);
	DDX_Control(pDX, IDC_EDT_NUM6, m_edtNum6);
	DDX_Control(pDX, IDC_BTN_MIN, m_btnMin);
	DDX_Control(pDX, IDC_BTN_BACK, m_btnBack);
}


BEGIN_MESSAGE_MAP(ClaDlgOtp, ClaDialogPng)
	ON_BN_CLICKED(IDOK, &ClaDlgOtp::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDT_NUM1, &ClaDlgOtp::OnEnChangeEdtNum1)
	ON_EN_CHANGE(IDC_EDT_NUM2, &ClaDlgOtp::OnEnChangeEdtNum2)
	ON_EN_CHANGE(IDC_EDT_NUM3, &ClaDlgOtp::OnEnChangeEdtNum3)
	ON_EN_CHANGE(IDC_EDT_NUM4, &ClaDlgOtp::OnEnChangeEdtNum4)
	ON_EN_CHANGE(IDC_EDT_NUM5, &ClaDlgOtp::OnEnChangeEdtNum5)
	ON_EN_CHANGE(IDC_EDT_NUM6, &ClaDlgOtp::OnEnChangeEdtNum6)
	ON_BN_CLICKED(IDC_BTN_MIN, &ClaDlgOtp::OnBnClickedBtnMin)
	ON_BN_CLICKED(IDC_BTN_BACK, &ClaDlgOtp::OnBnClickedBtnBack)
	ON_STN_CLICKED(IDC_STC_RESEND, &ClaDlgOtp::OnStnClickedStcResend)
END_MESSAGE_MAP()


// ClaDlgOtp message handlers


BOOL ClaDlgOtp::OnInitDialog()
{
	ClaDialogPng::OnInitDialog();

	// TODO:  Add extra initialization here
	HICON hIcon = theApp.LoadIconW(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - opt start");

	_read();

	_enableUI();

	CRect rt;
	GetDlgItem(IDCANCEL)->GetClientRect(rt);
	setTitleHeight(rt.Height());

	m_grpBG.SetBackgroundColor(GD_COLOR_GROUP_BG, GD_COLOR_GROUP_BG);

	m_stcLogo.LoadPng(IDB_PNG_LOGO);

	m_stcTitle.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, TRUE, 0);
	m_stcTitle.SetFontColor(GD_COLOR_LABEL);

	m_stcSubTitle.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TITLE, TRUE, 1);
	m_stcSubTitle.SetFontColor(GD_COLOR_LABEL);

	m_grpOTP.SetBackgroundColor(GD_COLOR_GROUP, GD_COLOR_GROUP);
	m_stcOTP.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 0);
	m_stcOTP.SetFontColor(GD_COLOR_LABEL);

	m_stcResend.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 2);
	m_stcResend.SetFontColor(COLOR_HYPER);

	m_btnOK.setColor(GD_COLOR_BTN_BG, GD_COLOR_BTN_BG, GD_COLOR_BTN_TXT);
	m_btnOK.setFont(GD_FONT_FACE, GD_FONT_SIZE_BTN, TRUE, 1);

	m_btnBack.setColor(GD_COLOR_BTN_BG1, GD_COLOR_BTN_BG1, GD_COLOR_BTN_TXT);
	m_btnBack.setFont(GD_FONT_FACE, GD_FONT_SIZE_BTN, TRUE, 1);

	m_btnCancel.setColor(GD_COLOR_DLG_TITLE, GD_COLOR_DLG_TITLE, GD_COLOR_BTN_BG);
	m_btnCancel.setFont(GD_FONT_FACE, GD_FONT_SIZE_BTN, TRUE, 1);
	m_btnCancel.setBgImage(IDB_PNG_EXIT);

	m_btnMin.setColor(GD_COLOR_DLG_TITLE, GD_COLOR_DLG_TITLE, GD_COLOR_BTN_BG);
	m_btnMin.setFont(GD_FONT_FACE, GD_FONT_SIZE_BTN, TRUE, 1);
	m_btnMin.setBgImage(IDB_PNG_MIN);

	m_edtNum1.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_EDIT_OTP, FALSE, 1);
	m_edtNum2.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_EDIT_OTP, FALSE, 1);
	m_edtNum3.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_EDIT_OTP, FALSE, 1);
	m_edtNum4.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_EDIT_OTP, FALSE, 1);
	m_edtNum5.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_EDIT_OTP, FALSE, 1);
	m_edtNum6.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_EDIT_OTP, FALSE, 1);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

extern HANDLE g_hProcNodeClient;

int lv_nCountOtp = 3;
void ClaDlgOtp::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (m_strNum1.IsEmpty()
	|| m_strNum2.IsEmpty()
	|| m_strNum3.IsEmpty()
	|| m_strNum4.IsEmpty()
	|| m_strNum5.IsEmpty()
	|| m_strNum6.IsEmpty()
	) {
		return;
	}


	CString opCode = m_strNum1 + m_strNum2 + m_strNum3 + m_strNum4 + m_strNum5 + m_strNum6;

	char szOpCode[10]; memset(szOpCode, 0, sizeof(szOpCode));
	sprintf_s(szOpCode, 10, "%S", opCode);

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - opt conn");
	if (g_strGlobalIP.IsEmpty()) {
		g_strGlobalIP = g_strIP;
	}
	int nSts = kc_otp(KGlobal::GetMachineID(), opCode, g_strGlobalIP, NULL, 0);

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - opt =%d[%d]", nSts, GetLastError());

	if (nSts != 0)
	{
		if (kc_get_err() == ERROR_SERVER_DISABLED) {
			AfxMessageBox(L"Unable to connect to the server, please check your internet connectivity or retry again later");
			return;
		}
		else {
			AfxMessageBox(L"Please input the correct OTP code.");
			if (lv_nCountOtp > 1) {
				lv_nCountOtp--;
				return;
			}
			else {
				OnCancel();
				return;
			}
		}
	}

	{
		STARTUPINFO si; memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;

		CString strPath; strPath.Format(L"%s\\kclient.exe", ClaPathMgr::GetDP());
		DWORD dwPID = ClaProcess::FindProcessID(L"kclient.exe");
		if (dwPID != 0) {
			TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID), 0);
		}

		char szID[10]; kc_get_uid(szID);

		char szSession[64]; kc_get_session(szSession, 64);
		CString strCmd; strCmd.Format(L"\"%s\" \"%s\" \"%S\" \"%S\"", strPath, g_strGlobalIP, szSession, szID);

		ClaProcess::CreateProcessEx(strPath, strCmd, ClaPathMgr::GetDP(),
			NULL, &g_hProcNodeClient, &si);
	}


	ClaDialogPng::OnOK();
}

void ClaDlgOtp::_read()
{
	char szIniPath[MAX_PATH]; memset(szIniPath, 0, sizeof(szIniPath));
	sprintf_s(szIniPath, "%S\\config.ini", ClaPathMgr::GetDP());

	mINI::INIFile file(szIniPath);
	mINI::INIStructure ini;

	file.read(ini);

	_strServer.Format(L"%S", ini["network"]["server"].c_str());
	_strPort.Format(L"%S", ini["network"]["port"].c_str());

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - opt [%s][%s]", _strServer, _strPort);
}


void ClaDlgOtp::OnEnChangeEdtNum1()
{
	UpdateData(TRUE);
	m_strNum1 = m_strNum1.Left(1);
	if (!m_strNum1.IsEmpty()) {
		GetDlgItem(IDC_EDT_NUM2)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDT_NUM2))->SetSel(0, -1);
	}
	UpdateData(FALSE);

	_enableUI();
}

void ClaDlgOtp::OnEnChangeEdtNum2()
{
	UpdateData(TRUE);
	m_strNum2 = m_strNum2.Left(1);
	if (!m_strNum2.IsEmpty()) {
		GetDlgItem(IDC_EDT_NUM3)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDT_NUM3))->SetSel(0, -1);
	}
	UpdateData(FALSE);
	_enableUI();
}

void ClaDlgOtp::OnEnChangeEdtNum3()
{
	UpdateData(TRUE);
	m_strNum3 = m_strNum3.Left(1);
	if (!m_strNum3.IsEmpty()) {
		GetDlgItem(IDC_EDT_NUM4)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDT_NUM4))->SetSel(0, -1);
	}
	UpdateData(FALSE);
	_enableUI();
}

void ClaDlgOtp::OnEnChangeEdtNum4()
{
	UpdateData(TRUE);
	m_strNum4 = m_strNum4.Left(1);
	if (!m_strNum4.IsEmpty()) {
		GetDlgItem(IDC_EDT_NUM5)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDT_NUM5))->SetSel(0, -1);
	}
	UpdateData(FALSE);
	_enableUI();
}

void ClaDlgOtp::OnEnChangeEdtNum5()
{
	UpdateData(TRUE);
	m_strNum5 = m_strNum5.Left(1);
	if (!m_strNum5.IsEmpty()) {
		GetDlgItem(IDC_EDT_NUM6)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDT_NUM6))->SetSel(0, -1);
	}
	UpdateData(FALSE);
	_enableUI();
}

void ClaDlgOtp::OnEnChangeEdtNum6()
{
	UpdateData(TRUE);
	m_strNum6 = m_strNum6.Left(1);
	UpdateData(FALSE);
	_enableUI();
}

void ClaDlgOtp::_enableUI()
{
	if (m_strNum1.IsEmpty()
		|| m_strNum2.IsEmpty()
		|| m_strNum3.IsEmpty()
		|| m_strNum4.IsEmpty()
		|| m_strNum5.IsEmpty()
		|| m_strNum6.IsEmpty()
		) {
//		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
	else {
//		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
}


void ClaDlgOtp::OnBnClickedBtnMin()
{
	ShowWindow(SW_MINIMIZE);
}


void ClaDlgOtp::OnBnClickedBtnBack()
{
	m_bBack = TRUE;
	OnCancel();
}


void ClaDlgOtp::OnStnClickedStcResend()
{
	int nSts = kc_auth(m_strUser, m_strPwd, KGlobal::GetMachineID());
	if (nSts == 0) {
		AfxMessageBox(L"New OTP code was sent to your email.", MB_ICONINFORMATION);
	}
	else {
		AfxMessageBox(L"Unable to connect to server.");
	}
}
