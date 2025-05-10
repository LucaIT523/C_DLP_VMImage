
// KUpdateApp.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "KUpdateApp.h"
#include "KUpdateAppDlg.h"
#include "ClaPathMgr.h"
#include "ClaProcess.h"
#include "ClaRegMgr.h"
#include <winsvc.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKUpdateAppApp

BEGIN_MESSAGE_MAP(CKUpdateAppApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CKUpdateAppApp construction

CKUpdateAppApp::CKUpdateAppApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CKUpdateAppApp object

CKUpdateAppApp theApp;


// CKUpdateAppApp initialization

BOOL CKUpdateAppApp::InitInstance()
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

	CString cmd;
	cmd = GetCommandLine();
	if (cmd.Find(L"--check") >= 0) {
		TerminateProcess(GetCurrentProcess(), 0x8000);
	}

	DWORD pid = ClaProcess::GetParentProcessId(0);
	WaitForSingleObject(OpenProcess(SYNCHRONIZE, FALSE, pid), 5000);

	updateFilesandRun();
	CKUpdateAppDlg dlg;
	m_pMainWnd = &dlg;

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

BOOL _StartService()
{
	// Open a handle to the service control manager
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL) {
		return FALSE;
	}

	// Open the service
	SC_HANDLE hService = OpenService(hSCManager, L"KenanAgentService", SERVICE_START);
	if (hService == NULL) {
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	// Start the service
	if (!StartService(hService, 0, NULL)) {
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	// Cleanup
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	return TRUE;
}

void CKUpdateAppApp::updateFilesandRun() {
	// Get the parent directory of the application
	CString strDir = ClaPathMgr::GetDP(ClaPathMgr::GetDP());

	///////////////////////////////////////////////////////////////////
	//
	//	Delete All files of parent directory
	//
	_deleteFiles(strDir);

	///////////////////////////////////////////////////////////////////
	//
	//  Copy All Files from current directory to parent directory
	//
	_copyFiles(ClaPathMgr::GetDP(), strDir);

	///////////////////////////////////////////////////////////////////
	//
	//  Run updated KLauncher32.exe
	//

	CString strPath = ClaPathMgr::GetDP();
	CString fileFlag;
	fileFlag.Format(L"%s\\need_update.flag", strPath);
	DeleteFile(fileFlag);
	CString strLauncherExe;

	CString strCmd = GetCommandLine();
	if (strCmd.Find(L"--agent") >= 0) {
		_StartService();
	}
	else {
		strLauncherExe.Format(L"%s\\KLauncher32.exe", strDir);

		ClaRegMgr reg(HKEY_LOCAL_MACHINE);
		reg.deleteValue(L"Software\\Kenan", L"update_flag");

		DWORD dwPID;
		ClaProcess::CreateProcessEx(strLauncherExe, L"", strDir, NULL, NULL);
	}
}

// Function to delete a file
BOOL CKUpdateAppApp::_deleteFiles(const wchar_t* p_wszPath) {
	CFileFind finder;
	CString strFind; strFind.Format(L"%s\\*", p_wszPath);
	BOOL bSts;
	bSts = finder.FindFile(strFind);
	while (bSts) {
		bSts = finder.FindNextFileW();
		if (finder.IsDirectory()) continue;
		DeleteFile(finder.GetFilePath());
	}
	return TRUE;
}

BOOL CKUpdateAppApp::_copyFiles(const wchar_t* p_wszSrc, const wchar_t* p_wszDst)
{
	CFileFind finder;
	CString strFind; strFind.Format(L"%s\\*", p_wszSrc);
	BOOL bSts;
	bSts = finder.FindFile(strFind);
	while (bSts) {
		bSts = finder.FindNextFileW();
		if (finder.IsDirectory()) continue;
		CString strDest; strDest.Format(L"%s\\%s", p_wszDst, ClaPathMgr::GetFN(finder.GetFilePath()));
		CopyFile(finder.GetFilePath(), strDest, FALSE);
	}
	return TRUE;
}
