// ClaDlgOtp.cpp : implementation file
//

#include "pch.h"
#include "KenanNet.h"
#include "afxdialogex.h"
#include "ClaDlgOtp.h"
#include "ini.h"
#include "ClaPathMgr.h"

#include "ServerConn.h"
#include "ClaProcess.h"
#include "SessionManager.h"

// ClaDlgOtp dialog

IMPLEMENT_DYNAMIC(ClaDlgOtp, CDialog)

ClaDlgOtp::ClaDlgOtp(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_OTP, pParent)
	, m_strNum1(_T(""))
	, m_strNum2(_T(""))
	, m_strNum3(_T(""))
	, m_strNum4(_T(""))
	, m_strNum5(_T(""))
	, m_strNum6(_T(""))
{

}

ClaDlgOtp::~ClaDlgOtp()
{
}

void ClaDlgOtp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_NUM1, m_strNum1);
	DDX_Text(pDX, IDC_EDT_NUM2, m_strNum2);
	DDX_Text(pDX, IDC_EDT_NUM3, m_strNum3);
	DDX_Text(pDX, IDC_EDT_NUM4, m_strNum4);
	DDX_Text(pDX, IDC_EDT_NUM5, m_strNum5);
	DDX_Text(pDX, IDC_EDT_NUM6, m_strNum6);
}


BEGIN_MESSAGE_MAP(ClaDlgOtp, CDialog)
	ON_BN_CLICKED(IDOK, &ClaDlgOtp::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDT_NUM1, &ClaDlgOtp::OnEnChangeEdtNum1)
	ON_EN_CHANGE(IDC_EDT_NUM2, &ClaDlgOtp::OnEnChangeEdtNum2)
	ON_EN_CHANGE(IDC_EDT_NUM3, &ClaDlgOtp::OnEnChangeEdtNum3)
	ON_EN_CHANGE(IDC_EDT_NUM4, &ClaDlgOtp::OnEnChangeEdtNum4)
	ON_EN_CHANGE(IDC_EDT_NUM5, &ClaDlgOtp::OnEnChangeEdtNum5)
	ON_EN_CHANGE(IDC_EDT_NUM6, &ClaDlgOtp::OnEnChangeEdtNum6)
END_MESSAGE_MAP()


// ClaDlgOtp message handlers


BOOL ClaDlgOtp::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	_read();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void ClaDlgOtp::OnBnClickedOk()
{
	UpdateData(TRUE);

	setServer(_strServer, _wtoi(_strPort));

	CString opCode = m_strNum1 + m_strNum2 + m_strNum3 + m_strNum4 + m_strNum5 + m_strNum6;

	char szOpCode[10]; memset(szOpCode, 0, sizeof(szOpCode));
	sprintf_s(szOpCode, 10, "%S", opCode);

	std::string strToken = otpAuth(szOpCode);

	if (strToken.empty()) {
		AfxMessageBox(L"It has been failed to authenticate.");
		return;
	}

	SessionManager sess;
	sess.setToken(strToken.c_str());
	sess.saveToken();

	CDialog::OnOK();
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
}


void ClaDlgOtp::OnEnChangeEdtNum1()
{
	UpdateData(TRUE);
	m_strNum1 = m_strNum1.Left(1);
	if (!m_strNum1.IsEmpty()) {
		GetDlgItem(IDC_EDT_NUM2)->SetFocus();
	}
	UpdateData(FALSE);
}

void ClaDlgOtp::OnEnChangeEdtNum2()
{
	UpdateData(TRUE);
	m_strNum2 = m_strNum2.Left(1);
	if (!m_strNum2.IsEmpty()) {
		GetDlgItem(IDC_EDT_NUM3)->SetFocus();
	}
	UpdateData(FALSE);
}

void ClaDlgOtp::OnEnChangeEdtNum3()
{
	UpdateData(TRUE);
	m_strNum3 = m_strNum3.Left(1);
	if (!m_strNum3.IsEmpty()) {
		GetDlgItem(IDC_EDT_NUM4)->SetFocus();
	}
	UpdateData(FALSE);
}

void ClaDlgOtp::OnEnChangeEdtNum4()
{
	UpdateData(TRUE);
	m_strNum4 = m_strNum4.Left(1);
	if (!m_strNum4.IsEmpty()) {
		GetDlgItem(IDC_EDT_NUM5)->SetFocus();
	}
	UpdateData(FALSE);
}

void ClaDlgOtp::OnEnChangeEdtNum5()
{
	UpdateData(TRUE);
	m_strNum5 = m_strNum5.Left(1);
	if (!m_strNum5.IsEmpty()) {
		GetDlgItem(IDC_EDT_NUM6)->SetFocus();
	}
	UpdateData(FALSE);
}

void ClaDlgOtp::OnEnChangeEdtNum6()
{
	UpdateData(TRUE);
	m_strNum6 = m_strNum6.Left(1);
	UpdateData(FALSE);
}

