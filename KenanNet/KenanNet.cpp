
// KenanNet.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "KenanNet.h"
#include "KenanNetDlg.h"

#include "ClaDlgLogin.h"
#include "ClaDlgConfig.h"
#include "ClaDlgDownload.h"
#include "ClaDlgOtp.h"
#include "SessionManager.h"
#include "ServerConn.h"
#include "ClaPathMgr.h"
#include "ini.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKenanNetApp

BEGIN_MESSAGE_MAP(CKenanNetApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CKenanNetApp construction

CKenanNetApp::CKenanNetApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CKenanNetApp object

CKenanNetApp theApp;

#define CODE_OK		(658)

void load_config()
{
	char szIniPath[MAX_PATH]; memset(szIniPath, 0, sizeof(szIniPath));
	sprintf_s(szIniPath, "%S\\config.ini", ClaPathMgr::GetDP());

	mINI::INIFile file(szIniPath);
	mINI::INIStructure ini;

	file.read(ini);

	
	CString _strServer; _strServer.Format(L"%S", ini["network"]["server"].c_str());
	CString _strPort; _strPort.Format(L"%S", ini["network"]["port"].c_str());

	setServer(_strServer, _wtoi(_strPort));
}
// CKenanNetApp initialization

BOOL CKenanNetApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CString strCmdLine = GetCommandLine();

	{
		if (strCmdLine.Find(L"--authentication") >= 0) {
			if (_auth() == 0) {
				TerminateProcess(GetCurrentProcess(), CODE_OK);
			}
		}
	}

	load_config();

	{
		if (strCmdLine.Find(L"--download-list") >= 0) {
			if (_down_list() == 0) {
				TerminateProcess(GetCurrentProcess(), CODE_OK);
			}
		}
	}

	{
		if (strCmdLine.Find(L"--download-unzip") >= 0) {
			if (_down_unzip() == 0) {
				TerminateProcess(GetCurrentProcess(), CODE_OK);
			}
		}
	}

	{
		if (strCmdLine.Find(L"--clear_credential") >= 0) {
			if (_logout() == 0) {
				TerminateProcess(GetCurrentProcess(), CODE_OK);
			}
		}
	}

	{
		if (strCmdLine.Find(L"--otp-authentication") >= 0) {
			if (_otp() == 0) {
				TerminateProcess(GetCurrentProcess(), CODE_OK);
			}
		}
	}


	if (0)
	{
		CKenanNetDlg dlg;
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
		}
		else if (nResponse == -1)
		{
			TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
			TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
		}
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

unsigned int CKenanNetApp::_auth()
{
	ClaDlgLogin dlg;
	if (dlg.DoModal() != IDOK) {
		return -1;
	}
	return 0;
}

SessionManager lv_net_manager;

void AddtoList(
	int p_nIndex,
	const char* p_szName,
	const char* p_szDesc,
	const char* p_szSize)
{
	lv_net_manager.addList(p_nIndex, p_szName, p_szDesc, p_szSize);
}

unsigned int CKenanNetApp::_down_list()
{
	lv_net_manager.loadToken();
	
	char szToken[200]; memset(szToken, 0, sizeof(szToken));
	lv_net_manager.getToken(szToken);
	if (szToken[0] == 0) {
		return -1;
	}

	std::string token = szToken;

	lv_net_manager.clearList();
	requestFileList(token, AddtoList);

	return 0;
}

unsigned int CKenanNetApp::_down_unzip()
{
	wchar_t wszCmd[512]; memset(wszCmd, 0, sizeof(wszCmd));
	wcscpy_s(wszCmd, 512, GetCommandLine());

	int nIndex = -1;
	int nArg;
	LPWSTR* pArg = CommandLineToArgvW(wszCmd, &nArg);
	for (int i = 0; i < nArg; i++) {
		if (wcscmp(pArg[i], L"--download-unzip") == 0) {
			nIndex = i + 1;
			break;
		}
	}
	if (nIndex >= nArg) return -1;

	ClaDlgDownload dlg;
	dlg.m_strName = pArg[nIndex];

	if (dlg.DoModal() != IDOK) {
		return -2;
	}

	return 0;
}

unsigned int CKenanNetApp::_logout()
{
	lv_net_manager.clearToken();
	return 0;
}

unsigned int CKenanNetApp::_otp()
{
	ClaDlgOtp dlg;
	if (dlg.DoModal() != IDOK) {
		return -1;
	}
	return 0;
}

