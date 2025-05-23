// ClaDlgLogin.cpp : implementation file
//

#include "pch.h"
#include "launcher.h"
#include "afxdialogex.h"
#include "ClaDlgLogin.h"
#include "ClaPathMgr.h"
#include "ini.h"
#include "ClaDlgConfig.h"
#include "ClaProcess.h"
#include "SessionManager.h"
#include "KC_common.h"
#include "ClaRegMgr.h"
#include "ClaNetAdapter.h"

// ClaDlgLogin dialog

IMPLEMENT_DYNAMIC(ClaDlgLogin, ClaDialogPng)

ClaDlgLogin::ClaDlgLogin(CWnd* pParent /*=nullptr*/)
	: ClaDialogPng(IDD_DLG_LOGIN, pParent)
	, m_strUser(_T(""))
	, m_strPwd(_T(""))
	, m_strMachineID(_T(""))
	, m_bRemember(FALSE)
	, m_strServer(_T(""))
{

}

ClaDlgLogin::~ClaDlgLogin()
{
}

void ClaDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	ClaDialogPng::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_USERNAME, m_strUser);
	DDX_Text(pDX, IDC_EDT_PASSWORD, m_strPwd);
	DDX_Control(pDX, IDC_BTN_SETTING, m_btnConfig);
	DDX_Text(pDX, IDC_EDT_MACHINE, m_strMachineID);
	DDX_Check(pDX, IDC_CHK_SAVEUSER, m_bRemember);
	DDX_Control(pDX, IDC_STC_ICON, m_stcIcon);
	DDX_Control(pDX, IDC_STC_MID, m_stcMid);
	DDX_Control(pDX, IDC_STC_MAIL, m_stcEmail);
	DDX_Control(pDX, IDC_STC_PWD, m_stcPwd);
	DDX_Control(pDX, IDC_STC_CLICK, m_stcClick);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_CHK_SAVEUSER, m_chkRemember);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_GRP_MACHINE, m_grpMachine);
	DDX_Control(pDX, IDC_GRP_EMAIL, m_grpMail);
	DDX_Control(pDX, IDC_GRP_PWD, m_grpPwd);
	DDX_Control(pDX, IDC_STC_GRP_BG, m_grpGB);
	DDX_Control(pDX, IDC_GRP_IP, m_grpIP);
	DDX_Control(pDX, IDC_STC_IP, m_stcIP);
	DDX_Control(pDX, IDC_CMB_IP, m_cmbIP);
	DDX_Control(pDX, IDC_STC_TITLE, m_stcTitle);
	DDX_Control(pDX, IDC_STC_SUBTITLE, m_stcSubTitle);
	DDX_Control(pDX, IDC_EDT_MACHINE, m_edtMachine);
	DDX_Control(pDX, IDC_EDT_USERNAME, m_edtMail);
	DDX_Control(pDX, IDC_EDT_PASSWORD, m_edtPwd);
	DDX_Control(pDX, IDC_BTN_MIN, m_btnMin);
	DDX_Control(pDX, IDC_BTN_EYE, m_btnEye);
	DDX_Control(pDX, IDC_EDT_SERVER, m_edtServer);
	DDX_Text(pDX, IDC_EDT_SERVER, m_strServer);
	DDX_Control(pDX, IDC_GRP_SERVER, m_grpServer);
}


BEGIN_MESSAGE_MAP(ClaDlgLogin, ClaDialogPng)
	ON_BN_CLICKED(IDC_BTN_SETTING, &ClaDlgLogin::OnBnClickedBtnSetting)
	ON_BN_CLICKED(IDOK, &ClaDlgLogin::OnBnClickedOk)
	ON_STN_CLICKED(IDC_STC_CLICK, &ClaDlgLogin::OnStnClickedStcClick)
	ON_BN_CLICKED(IDC_BTN_MIN, &ClaDlgLogin::OnBnClickedBtnMin)
END_MESSAGE_MAP()


// ClaDlgLogin message handlers


BOOL ClaDlgLogin::OnInitDialog()
{
	ClaDialogPng::OnInitDialog();

	HICON hIcon = theApp.LoadIconW(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	// TODO:  Add extra initialization here
	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - Login start");

	_read();

	_loadPassword();

	m_strMachineID = KGlobal::GetMachineID();

	UpdateData(FALSE);

	if (_strServer.IsEmpty() || _strPort.IsEmpty()) {
		OnBnClickedBtnSetting();
	}
	if (_strServer.IsEmpty() || _strPort.IsEmpty()) {
		EndDialog(IDCANCEL);
	}

	_init_ui();

	_init_ip();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CB_MOUSE_EYE(void* p, BOOL p_bFlg)
{
	ClaDlgLogin* pThis = (ClaDlgLogin*)p;
	if (p_bFlg) {
		((CEdit*)pThis->GetDlgItem(IDC_EDT_PASSWORD))->SetPasswordChar(0);
		((CEdit*)pThis->GetDlgItem(IDC_EDT_PASSWORD))->Invalidate(FALSE);
	}
	else {
		((CEdit*)pThis->GetDlgItem(IDC_EDT_PASSWORD))->SetPasswordChar(L'●');
		((CEdit*)pThis->GetDlgItem(IDC_EDT_PASSWORD))->Invalidate(FALSE);
	}
}

void ClaDlgLogin::_init_ui() {

	CRect rt;
	GetDlgItem(IDCANCEL)->GetClientRect(rt);
	setTitleHeight(rt.Height());

	m_btnConfig.SetIcon(theApp.LoadIconW(IDI_CONFIG));

	m_grpGB.SetBackgroundColor(GD_COLOR_GROUP_BG, GD_COLOR_GROUP_BG);

	m_stcIcon.LoadPng(IDB_PNG_LOGO);

	m_stcTitle.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, TRUE, 0);
	m_stcTitle.SetFontColor(GD_COLOR_LABEL);

	m_stcSubTitle.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TITLE, TRUE, 1);
	m_stcSubTitle.SetFontColor(GD_COLOR_LABEL);

	m_grpMachine.SetBackgroundColor(GD_COLOR_GROUP, GD_COLOR_GROUP);
	m_stcMid.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 0);
	m_stcMid.SetFontColor(GD_COLOR_LABEL);

	m_edtMachine.SetBackgroundColor(RGB(0, 255, 255), GD_COLOR_GROUP_BG);
	m_edtMachine.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_EDIT, FALSE, 1);
	m_edtMachine.SetFontColor(RGB(255, 0, 0));

	m_grpMail.SetBackgroundColor(GD_COLOR_GROUP, GD_COLOR_GROUP);
	m_stcEmail.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 0);
	m_stcEmail.SetFontColor(GD_COLOR_LABEL);
	m_edtMail.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_EDIT, FALSE, 1);
	m_edtMail.SetFontColor(GD_COLOR_LABEL);

	m_grpPwd.SetBackgroundColor(GD_COLOR_GROUP, GD_COLOR_GROUP);
	m_stcPwd.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 0);
	m_stcPwd.SetFontColor(GD_COLOR_LABEL);
	
	m_edtPwd.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_EDIT, FALSE, 1);
	m_edtPwd.SetFontColor(GD_COLOR_LABEL);

	m_grpIP.SetBackgroundColor(GD_COLOR_GROUP, GD_COLOR_GROUP);
	m_stcIP.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 0);
	m_stcIP.SetFontColor(GD_COLOR_LABEL);

	m_chkRemember.setColor(GD_COLOR_GROUP_BG, GD_COLOR_GROUP_BG, GD_COLOR_LABEL);
	m_chkRemember.setFont(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 0);
	m_chkRemember.setImage(IDB_PNG_CHK, IDB_PNG_UNCHK);

	m_stcClick.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_TEXT, FALSE, 2);
	m_stcClick.SetFontColor(COLOR_HYPER);

	m_btnOK.setColor(GD_COLOR_BTN_BG, GD_COLOR_BTN_BG, GD_COLOR_BTN_TXT);
	m_btnOK.setFont(GD_FONT_FACE, GD_FONT_SIZE_BTN, TRUE, 1);

	m_btnCancel.setColor(GD_COLOR_DLG_TITLE, GD_COLOR_DLG_TITLE, GD_COLOR_BTN_BG);
	m_btnCancel.setFont(GD_FONT_FACE, GD_FONT_SIZE_BTN, TRUE, 1);
	m_btnCancel.setBgImage(IDB_PNG_EXIT);

	m_btnMin.setColor(GD_COLOR_DLG_TITLE, GD_COLOR_DLG_TITLE, GD_COLOR_BTN_BG);
	m_btnMin.setBgImage(IDB_PNG_MIN);

	m_btnEye.setColor(GD_COLOR_GROUP, GD_COLOR_GROUP, GD_COLOR_BTN_BG);
	m_btnEye.setBgImage(IDB_PNG_EYE);
	m_btnEye.setPressingCB(CB_MOUSE_EYE, this);

	m_btnConfig.setColor(GD_COLOR_GROUP, GD_COLOR_GROUP, GD_COLOR_BTN_BG);
	m_btnConfig.setBgImage(IDB_PNG_BTN_SETTING);

	m_grpServer.SetBackgroundColor(GD_COLOR_GROUP, GD_COLOR_GROUP);
	m_edtServer.SetBackgroundColor(RGB(0, 255, 255), GD_COLOR_GROUP_BG);
	m_edtServer.SetFontStyle(GD_FONT_FACE, GD_FONT_SIZE_EDIT, FALSE, 1);
	m_edtServer.SetFontColor(RGB(255, 0, 0));

}

void ClaDlgLogin::_init_ip()
{
	CString strIPs = ClaNetAdapter::GetIPs();
	strIPs.Trim(L"|");  strIPs.Replace(L"|", L" ");

	int count;
	LPWSTR* pIPs = CommandLineToArgvW(strIPs, &count);

	for (int i = 0; i < count; i++) {
		m_cmbIP.AddString(pIPs[i]);
	}

	if (count > 0) {
		m_cmbIP.SetCurSel(0);
	}
	else {
		AfxMessageBox(L"Cannot find any network adapter on your computer.\nPlease check it and try again.");
		TerminateProcess(GetCurrentProcess(), 0);
	}
}

void ClaDlgLogin::_read()
{
	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - Login load server conf start");

	char szIniPath[MAX_PATH]; memset(szIniPath, 0, sizeof(szIniPath));
	sprintf_s(szIniPath, "%S\\config_client.ini", ClaPathMgr::GetDP());

	mINI::INIFile file(szIniPath);
	mINI::INIStructure ini;

	file.read(ini);

	_strServer.Format(L"%S", ini["settings"]["gw_ip"].c_str());
	_strPort.Format(L"%S", ini["settings"]["gw_port"].c_str());
	g_nPeriod = atoi(ini["settings"]["period_update"].c_str());
	if (g_nPeriod == 0) {
		g_nPeriod = 30;
	}

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - Login load server conf end server=[%s], port=[%s]", _strServer, _strPort);

	m_strServer = _strServer;
}
void ClaDlgLogin::_write()
{
	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - Login write server conf start [%s][%s]", _strServer, _strPort);

	char szIniPath[MAX_PATH]; memset(szIniPath, 0, sizeof(szIniPath));
	sprintf_s(szIniPath, "%S\\config_client.ini", ClaPathMgr::GetDP());

	mINI::INIFile file(szIniPath);
	mINI::INIStructure ini;

	file.read(ini);

	char szServer[200]; memset(szServer, 0, sizeof(szServer));
	char szPort[200]; memset(szPort, 0, sizeof(szPort));

	sprintf_s(szServer, 200, "%S", _strServer);
	sprintf_s(szPort, 200, "%S", _strPort);

	ini["settings"]["gw_ip"] = szServer;
	ini["settings"]["gw_port"] = szPort;

	file.generate(ini);

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - Login write server conf end");
}

extern HANDLE g_hProcNodeClient;

void ClaDlgLogin::OnBnClickedBtnSetting()
{
	UpdateData(TRUE);

	ClaDlgConfig dlg;
	dlg.setColor(GD_COLOR_DLG_BG, GD_COLOR_DLG_BD, GD_COLOR_DLG_TITLE);

	dlg.m_strServer = _strServer;
	dlg.m_strPort = _strPort;

	ShowWindow(SW_HIDE);

	if (dlg.DoModal() == IDOK) {
		_strServer = dlg.m_strServer;
		_strPort = dlg.m_strPort;

		_write();

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

			CString strCmd; strCmd.Format(L"\"%s\" \"%s\" \"%s\" \"\"", strPath, g_strGlobalIP, L"");

			ClaProcess::CreateProcessEx(strPath, strCmd, ClaPathMgr::GetDP(),
				NULL, &g_hProcNodeClient, &si);
		}

	}

	ShowWindow(SW_SHOW);

	m_strServer = _strServer;
	UpdateData(FALSE);
}

void ClaDlgLogin::OnBnClickedOk()
{
	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - Login onok start");

	UpdateData(TRUE);

	int nSelCmb = m_cmbIP.GetCurSel();
	CString strIP; m_cmbIP.GetLBText(nSelCmb, strIP);
	g_strIP = strIP;

	//	127.0.0.1 is node_client listen.
	//	node_client will connect to gw and it will redirect to policy server
	if (kc_init(L"127.0.0.1", 43210) != 0) {
	//if (kc_init(_strServer, _wtoi(_strPort)) != 0) {
		g_pLog->LogFmtW(LOG_LEVEL_WARNING, L"Launcher - Login onok failed init");
		AfxMessageBox(L"Failed to initialize the kenan network connector.");
		return;
	}

	std::string globalip = kc_get_global_ip();

	g_strGlobalIP.Format(L"%S", globalip.c_str());
	if (g_strGlobalIP == L"188.43.33.249")
		g_strGlobalIP = L"172.92.33.42";

	char szUser[200]; memset(szUser, 0, sizeof(szUser));
	sprintf_s(szUser, 200, "%S", m_strUser);
	char szPwd[200]; memset(szPwd, 0, sizeof(szPwd));
	sprintf_s(szPwd, 200, "%S", m_strPwd);

	int nSts = kc_auth(m_strUser, m_strPwd, KGlobal::GetMachineID());
	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - Login onok auth = %d[%d]", nSts, GetLastError());
	if (nSts != 0)
	{
		if (kc_get_err() == ERROR_SERVER_DISABLED) {
			AfxMessageBox(L"Unable to connect to the server, please check your internet connectivity or retry again later");
			return;
		}
		else {
			wchar_t wszMessage[260]; 
			kc_get_message(wszMessage, 260);
			if (wszMessage[0] == 0) {
				AfxMessageBox(L"Unable to connect to the server, please check your internet connectivity or retry again later");
			}else
				AfxMessageBox(wszMessage);
			return;
		}
	}

	_savePassword();

	theApp.m_strServer = _strServer;
	ClaDialogPng::OnOK();
}
#include "kncrypto.h"

void ClaDlgLogin::_savePassword()
{
	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - Login save 1 start");

	ClaRegMgr reg(HKEY_LOCAL_MACHINE);

	char szData[512];
	memset(szData, 0, sizeof(szData));
	sprintf_s(szData, 512, "{\"n\":\"%S\",\"p\":\"\"}", m_strUser.GetBuffer());
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

void ClaDlgLogin::_loadPassword()
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

		m_strUser.Format(L"%S", name.c_str());
		m_strPwd.Format(L"%S", pwd.c_str());
	}
	catch (std::exception e) {
		return ;
	}

	m_bRemember = TRUE;
	UpdateData(FALSE);

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher - Login load 1 end success");
}


void ClaDlgLogin::OnStnClickedStcClick()
{
	ClaDlgConfig dlg;
	dlg.setColor(GD_COLOR_DLG_BG, GD_COLOR_DLG_BD, GD_COLOR_DLG_TITLE);

	dlg.m_strServer = _strServer;
	dlg.m_strPort = _strPort;

	ShowWindow(SW_HIDE);

	if (dlg.DoModal() == IDOK) {
		_strServer = dlg.m_strServer;
		_strPort = dlg.m_strPort;

		_write();
	}

	ShowWindow(SW_SHOW);
}


void ClaDlgLogin::OnBnClickedBtnMin()
{
	ShowWindow(SW_MINIMIZE);
}
