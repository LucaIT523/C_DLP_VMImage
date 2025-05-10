
// launcher.cpp : Defines the class behaviors for the application.
//

#include "pch.h"

#include "framework.h"
#include "launcher.h"
#include "launcherDlg.h"

#include "ClaProcess.h"
#include "ClaRegMgr.h"
#include "ClaPathMgr.h"

#include "ClaDlgLogin.h"
#include "ClaDlgOtp.h"
#include "KGlobal.h"
#include "ClaProcess.h"
#include "KC_common.h"
#include "ClaNetAdapter.h"
#include "ClaFwMgr.h"
#include "ClaDlg64.h"
#include "ClaRegMgr.h"
#include "ClaOsInfo.h"

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ClauncherApp

BEGIN_MESSAGE_MAP(ClauncherApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// ClauncherApp construction

ClauncherApp::ClauncherApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only ClauncherApp object

ClauncherApp theApp;
KGlobal g_kenan;
KVmMgr* g_pVmMgr = NULL;
ClaLogMgr* g_pLog = NULL;
CString g_strIP;
HANDLE g_hProcNodeClient = NULL;
CString g_strGlobalIP;
int g_nPeriod;

// ClauncherApp initialization


BOOL ClauncherApp::InitInstance()
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

	//CString str = ClaNetAdapter::GetIP();
	//AfxMessageBox(str);
	//TerminateProcess(GetCurrentProcess(), 0);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	g_pLog = new ClaLogMgr();
	g_pLog->loadFromIni();

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher v1.0.0.0 started.");

	ULONG_PTR m_gdiplusToken;
	GdiplusStartupInput m_gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, nullptr);

	// Initialize Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
	}


#ifdef WIN64
	{
		ClaDlg64 dlg;
		dlg.DoModal();
	}

	goto L_EXIT;
#endif

	if (checkEnv() != 0) {
		g_pLog->LogFmtW(LOG_LEVEL_WARNING, L"Launcher failed to initialize");
		goto L_EXIT;
	}

L_RESTART:
	{
		g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher show login dialog");

		ClaDlgLogin dlg;
		//dlg.setBackground(IDB_PNG_BG);
		dlg.setColor(GD_COLOR_DLG_BG, GD_COLOR_DLG_BD, GD_COLOR_DLG_TITLE);
		if (dlg.DoModal() != IDOK) {
			goto L_EXIT;
		}

		g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher show opt dialog");

		ClaDlgOtp dlg1;
		dlg1.m_strUser = dlg.m_strUser;
		dlg1.m_strPwd = dlg.m_strPwd;

		dlg1.setColor(GD_COLOR_DLG_BG, GD_COLOR_DLG_BD, GD_COLOR_DLG_TITLE);
		if (dlg1.DoModal() != IDOK) {
			if (dlg1.m_bBack) {
				goto L_RESTART;
			}
			goto L_EXIT;
		}
	}

	{
		g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher show main dialog");

		ClauncherDlg dlg;
		dlg.setColor(GD_COLOR_DLG_BG, GD_COLOR_DLG_BD, GD_COLOR_DLG_TITLE);
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

L_EXIT:
	WSACleanup();

	GdiplusShutdown(m_gdiplusToken);

	if (kc_uninit != NULL) kc_uninit();
	if (g_pVmMgr != NULL) {
		delete g_pVmMgr;
	}
	if (g_hProcNodeClient != NULL) TerminateProcess(g_hProcNodeClient, 0);
	if (g_pLog) delete g_pLog;
	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif
	TerminateProcess(GetCurrentProcess(), 0);
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

unsigned int ClauncherApp::checkEnv()
{
	wchar_t wszDllPath[MAX_PATH]; memset(wszDllPath, 0, sizeof(wszDllPath));
	wchar_t wszPlayerPath[MAX_PATH]; memset(wszPlayerPath, 0, sizeof(wszPlayerPath));
	wchar_t wszWorkstationPath[MAX_PATH]; memset(wszWorkstationPath, 0, sizeof(wszWorkstationPath));

	BOOL bVmPlayerInstalled = false;
	BOOL bVmwareInstalled = false;

L_CHK_VM_INSTALLED:
	{
		//.	check vmware workstation or vmwareplayer
		ClaRegMgr reg(HKEY_LOCAL_MACHINE);

		if (reg.readStringW(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\vmplayer.exe", L"", wszPlayerPath) != 0) {
			bVmPlayerInstalled = false;
		}
		else {
			bVmPlayerInstalled = true;
		}

		if (reg.readStringW(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\vmware.exe", L"", wszWorkstationPath) != 0) {
			bVmwareInstalled = false;
		}
		else {
			bVmwareInstalled = true;
		}

		if (bVmPlayerInstalled == false && bVmwareInstalled == false) {
			AfxMessageBox(L"VMware is not installed on this computer. Please install VMware first.");
			return -2;
		}
	}

L_CHK_VM_LAUNCH:
	{
		g_pVmMgr = new KVmMgr();
	}

L_CHK_NODE_CLIENT:
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

		if (GetFileAttributes(strPath) == INVALID_FILE_ATTRIBUTES) {
			AfxMessageBox(L"Some files were missing.");
			return -3;
		}

		ClaFwMgr::DeleteRule(L"KenanClientRule", TRUE);
		ClaFwMgr::AddAppRule(L"KenanClientRule", L"Application rule for kenan client", strPath.GetBuffer(), TRUE);

		CString strCmd; strCmd.Format(L"\"%s\" \"%s\" \"%s\" \"\"", strPath, g_strGlobalIP, L"");

		if (ClaProcess::CreateProcessEx(strPath, strCmd, ClaPathMgr::GetDP(),
			NULL, &g_hProcNodeClient, &si) != 0) {
			AfxMessageBox(L"Some files were correupted.");
			return -4;
		}
	}
	return 0;
}
