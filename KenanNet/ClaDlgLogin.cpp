// ClaDlgLogin.cpp : implementation file
//

#include "pch.h"
#include "KenanNet.h"
#include "afxdialogex.h"
#include "ClaDlgLogin.h"
#include "ClaPathMgr.h"
#include "ini.h"
#include "ClaDlgConfig.h"
#include "ServerConn.h"
#include "ClaProcess.h"
#include "SessionManager.h"

// ClaDlgLogin dialog

IMPLEMENT_DYNAMIC(ClaDlgLogin, CDialog)

ClaDlgLogin::ClaDlgLogin(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_LOGIN, pParent)
	, m_strUser(_T(""))
	, m_strPwd(_T(""))
{

}

ClaDlgLogin::~ClaDlgLogin()
{
}

void ClaDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_USERNAME, m_strUser);
	DDX_Text(pDX, IDC_EDT_PASSWORD, m_strPwd);
}


BEGIN_MESSAGE_MAP(ClaDlgLogin, CDialog)
	ON_BN_CLICKED(IDC_BTN_SETTING, &ClaDlgLogin::OnBnClickedBtnSetting)
	ON_BN_CLICKED(IDOK, &ClaDlgLogin::OnBnClickedOk)
END_MESSAGE_MAP()


// ClaDlgLogin message handlers


BOOL ClaDlgLogin::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	_read();

	if (_strServer.IsEmpty() || _strPort.IsEmpty()) {
		OnBnClickedBtnSetting();
	}
	if (_strServer.IsEmpty() || _strPort.IsEmpty()) {
		EndDialog(IDCANCEL);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void ClaDlgLogin::_read()
{
	char szIniPath[MAX_PATH]; memset(szIniPath, 0, sizeof(szIniPath));
	sprintf_s(szIniPath, "%S\\config.ini", ClaPathMgr::GetDP());

	mINI::INIFile file(szIniPath);
	mINI::INIStructure ini;

	file.read(ini);

	_strServer.Format(L"%S", ini["network"]["server"].c_str());
	_strPort.Format(L"%S", ini["network"]["port"].c_str());
}
void ClaDlgLogin::_write()
{
	char szIniPath[MAX_PATH]; memset(szIniPath, 0, sizeof(szIniPath));
	sprintf_s(szIniPath, "%S\\config.ini", ClaPathMgr::GetDP());

	mINI::INIFile file(szIniPath);
	mINI::INIStructure ini;

	char szServer[200]; memset(szServer, 0, sizeof(szServer));
	char szPort[200]; memset(szPort, 0, sizeof(szPort));

	sprintf_s(szServer, 200, "%S", _strServer);
	sprintf_s(szPort, 200, "%S", _strPort);

	ini["network"]["server"] = szServer;
	ini["network"]["port"] = szPort;

	file.generate(ini);

	///////////////////////
	FILE* pFile; _wfopen_s(&pFile, ClaPathMgr::GetDP() + L"\\server_info.dat", L"wb");
	fwrite(szServer, 1, strlen(szServer), pFile);
	fclose(pFile);
}

void ClaDlgLogin::OnBnClickedBtnSetting()
{
	ClaDlgConfig dlg;
	dlg.m_strServer = _strServer;
	dlg.m_strPort = _strPort;

	if (dlg.DoModal() == IDOK) {
		_strServer = dlg.m_strServer;
		_strPort = dlg.m_strPort;

		_write();
	}
}

void ClaDlgLogin::OnBnClickedOk()
{
	UpdateData(TRUE);

	setServer(_strServer, _wtoi(_strPort));

	char szUser[200]; memset(szUser, 0, sizeof(szUser));
	sprintf_s(szUser, 200, "%S", m_strUser);
	char szPwd[200]; memset(szPwd, 0, sizeof(szPwd));
	sprintf_s(szPwd, 200, "%S", m_strPwd);

	std::string strToken = authenticateUser(szUser, szPwd);
	if (strToken.empty()) {
		AfxMessageBox(L"It has been failed to authenticate.");
		return;
	}

	CDialog::OnOK();
}
