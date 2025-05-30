// ClaPageLogin.cpp : implementation file
//

#include "pch.h"
#include "KLauncher.h"
#include "KLauncherDlg.h"
#include "afxdialogex.h"
#include "ClaPageLogin.h"
#include "KGlobal.h"
#include "ClaRegMgr.h"
#include "kncrypto.h"

#define LD_MESSAGE_ON_CONFIG	(WM_USER+1)
CString g_strMail;
// ClaPageLogin dialog

ClaPageLogin::ClaPageLogin(CWnd* pParent /*=nullptr*/)
	: KDialog(IDD_PAGE_LOGIN, 0, L"PNG", 0, pParent, FALSE)
	, m_strMID(_T(""))
	, m_strMail(_T(""))
	, m_strPwd(_T(""))
	, m_strIP(_T(""))
	, m_bRemember(FALSE)
{

}

ClaPageLogin::~ClaPageLogin()
{
}

void ClaPageLogin::DoDataExchange(CDataExchange* pDX)
{
	KDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STC_LOGO, m_stcLogo);
	DDX_Control(pDX, IDC_STC_TITLE, m_stcTitle);
	DDX_Control(pDX, IDC_STC_TITLE2, m_stcTitle2);
	DDX_Control(pDX, IDC_STC_TITLE3, m_stcTitle3);
	DDX_Control(pDX, IDC_STC_PANEL, m_stcPanel);
	DDX_Control(pDX, IDC_EDT_MID, m_edtMID);
	DDX_Control(pDX, IDC_EDT_EMAIL, m_edtMail);
	DDX_Control(pDX, IDC_EDT_PWD, m_edtPwd);
	DDX_Control(pDX, IDC_EDT_IP, m_edtIP);
	DDX_Control(pDX, IDC_CHK_REMEMBER, m_chkRemember);
	DDX_Control(pDX, IDC_BTN_LOGIN, m_btnSignin);
	DDX_Text(pDX, IDC_EDT_MID, m_strMID);
	DDX_Text(pDX, IDC_EDT_EMAIL, m_strMail);
	DDX_Text(pDX, IDC_EDT_PWD, m_strPwd);
	DDX_Text(pDX, IDC_EDT_IP, m_strIP);
	DDX_Check(pDX, IDC_CHK_REMEMBER, m_bRemember);
}

BEGIN_MESSAGE_MAP(ClaPageLogin, KDialog)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &ClaPageLogin::OnBnClickedBtnLogin)
END_MESSAGE_MAP()

// ClaPageLogin message handlers
BOOL ClaPageLogin::OnInitDialog()
{
	KDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	_loadCredential();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void ClaPageLogin::OnInitChildrenWnds() {
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

	m_stcPanel.LoadImageK(IDB_PNG_PANEL_LOGIN);
	RegisterTranslucentWnd(&m_stcPanel);

	UINT nImgIdEdit[2] = { IDB_PNG_EDT_UNSEL, IDB_PNG_EDT_SEL };
	m_edtMID.LoadImageK(nImgIdEdit);
	m_edtMID.setFont(L"Arial", 10, FALSE, RGB(0xA4, 0xA4, 0xA4), RGB(0xFF, 0xFF, 0xFF));
	m_edtMID.setAlign(0);
	m_edtMID.SetIconImage(IDB_PNG_ICON_MID);
	RegisterTranslucentWnd(&m_edtMID);

	m_edtMail.LoadImageK(nImgIdEdit);
	m_edtMail.setFont(L"Arial", 10, FALSE, RGB(0xA4, 0xA4, 0xA4), RGB(0xFF, 0xFF, 0xFF));
	m_edtMail.setAlign(0);
	m_edtMail.SetIconImage(IDB_PNG_ICON_MAIL);
	RegisterTranslucentWnd(&m_edtMail);

	m_edtPwd.LoadImageK(nImgIdEdit);
	m_edtPwd.setFont(L"Arial", 10, FALSE, RGB(0xA4, 0xA4, 0xA4), RGB(0xFF, 0xFF, 0xFF));
	m_edtPwd.setAlign(0);
	m_edtPwd.SetIconImage(IDB_PNG_ICON_PWD);
	m_edtPwd.setPasswordChar(L'●');
	m_edtPwd.SetRightIcon(IDB_PNG_ICON_EYE);
	RegisterTranslucentWnd(&m_edtPwd);

	m_edtIP.LoadImageK(nImgIdEdit);
	m_edtIP.setFont(L"Arial", 10, FALSE, RGB(0xA4, 0xA4, 0xA4), RGB(0xFF, 0xFF, 0xFF));
	m_edtIP.setAlign(0);
	m_edtIP.SetIconImage(IDB_PNG_ICON_IP);
	m_edtIP.SetRightIcon(IDB_PNG_ICON_CONFIG, L"PNG", 0, LD_MESSAGE_ON_CONFIG);
	RegisterTranslucentWnd(&m_edtIP);

	UINT nImgIdChk[2] = { IDB_PNG_CHECK, IDB_PNG_UNCHECK};
	m_chkRemember.LoadImageList(nImgIdChk);
	m_chkRemember.setFont(L"Arial", 11, FALSE, RGB(255, 255, 255));
	m_chkRemember.setAlign(0);
	RegisterTranslucentWnd(&m_chkRemember);

	UINT nImgIdSignin[TWS_BUTTON_NUM] = { IDB_PNG_BTN_FULL, IDB_PNG_BTN_FULL_H, IDB_PNG_BTN_FULL_H, 0 };
	m_btnSignin.LoadImageList(nImgIdSignin);
	m_btnSignin.setFont(L"Arial", 12, FALSE, RGB(0xFF, 0xFF, 0xFF));
	m_btnSignin.setAlign(1);
	RegisterTranslucentWnd(&m_btnSignin);

	UpdateView();
}

void ClaPageLogin::OnBnClickedBtnLogin()
{
	UpdateData(TRUE);

	if (m_strMail.IsEmpty()) {
		KMessageBox(L"Please input the email address.");
		return;
	}

	if (m_strPwd.IsEmpty()) {
		KMessageBox(L"Please input the password");
		return;
	}

	if (m_strIP.IsEmpty()) {
		KMessageBox(L"Please config the server ip.");
		return;
	}

	if (m_nPort == 0) {
		KMessageBox(L"Please config the port.");
		return;
	}

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - Login onok start");
	//if (kc_init(L"127.0.0.1", 43210) != 0) {
	if (kc_init(m_strIP, m_nPort) != 0) {
		g_pLog->LogFmtW(LOG_LEVEL_WARNING, L"Launcher - Login onok failed init");
		KMessageBox(L"Failed to initialize the kenan network connector.");
		return;
	}

	int nSts = kc_auth(m_strMail, m_strPwd, g_strMachineID);
	g_strMail = m_strMail;
	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - Login onok auth = %d[%d]", nSts, GetLastError());
	if (nSts != 0)
	{
		if (kc_get_err() == ERROR_SERVER_DISABLED) {
			KMessageBox(L"Unable to connect to the server, please check your internet connectivity or retry again later");
			return;
		}
		else {
			wchar_t wszMessage[260];
			kc_get_message(wszMessage, 260);
			if (wszMessage[0] == 0) {
				KMessageBox(L"Unable to connect to the server, please check your internet connectivity or retry again later");
			}
			else
				KMessageBox(wszMessage);
			return;
		}
	}

	_saveCredential();

	g_pMain->ShowOtpPage();
}

void ClaPageLogin::OnShowPage() {

	wchar_t wszIP[64]; wchar_t wszPort[10];

	KGlobal::readServerIP(wszIP, 64, wszPort, 10, &g_nPolicyUpdatePeriod);
	m_strIP = wszIP;
	m_nPort = _wtoi(wszPort);
	m_strMID = g_strMachineID;
	m_strPwd = L"";
	UpdateData(FALSE);
}

LRESULT ClaPageLogin::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == LD_MESSAGE_ON_CONFIG) {
		UpdateData();
		g_pMain->ShowConfigPage();
	}

	return KDialog::WindowProc(message, wParam, lParam);
}

void ClaPageLogin::_saveCredential()
{
	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - Login save 1 start");

	ClaRegMgr reg(HKEY_LOCAL_MACHINE);

	char szData[512];
	memset(szData, 0, sizeof(szData));
	sprintf_s(szData, 512, "{\"n\":\"%S\",\"p\":\"\"}", m_strMail.GetBuffer());
	int l = strlen(szData);

	kn_encrypt_with_pwd((unsigned char*)szData, 512, L"20241119-kenan");
	reg.createKey(L"Software\\kenan");
	if (m_bRemember)
		reg.writeBinary(L"Software\\kenan", L"remember", (unsigned char*)szData, 512);
	else
		reg.deleteValue(L"Software\\kenan", L"remember");

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - Login save 1 end");
}

#include "json.hpp"

using json = nlohmann::json;

void ClaPageLogin::_loadCredential()
{
	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - Login load 1 start");

	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	DWORD l = 512;
	char szData[512];
	memset(szData, 0, sizeof(szData));

	if (reg.readBinary(L"Software\\kenan", L"remember", (unsigned char*)szData, &l) != 0) {
		g_pLog->LogFmtW(LOG_LEVEL_WARNING, L"Launcher - Login load 1 no data");
		return;
	}

	kn_decrypt_with_pwd((unsigned char*)szData, 512, L"20241119-kenan");

	try {
		json rsp = json::parse(szData);
		if (!rsp.contains("n") || !rsp.contains("p")) return;
		std::string name = rsp["n"];
		std::string pwd = rsp["p"];

		m_strMail.Format(L"%S", name.c_str());
//		m_strPwd.Format(L"%S", pwd.c_str());
	}
	catch (std::exception e) {
		return;
	}

	m_bRemember = TRUE;
	UpdateData(FALSE);

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - Login load 1 end success");
}
