
// KAgentQR.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "KAgentQR.h"
#include "KAgentQRDlg.h"
#include "ClaProcess.h"
#include "ClaDlgAlert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;
// CKAgentQRApp

BEGIN_MESSAGE_MAP(CKAgentQRApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CKAgentQRApp construction

CKAgentQRApp::CKAgentQRApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CKAgentQRApp object

CKAgentQRApp theApp;

UINT KMessageBox(const wchar_t* p_wszMessage, UINT p_nType) {
	ClaDlgAlert dlg(IDB_PNG_BG_ALERT);

	dlg.setAlertText(p_wszMessage);
	if (p_nType == MB_OKCANCEL) {
		dlg.setCancelMode(TRUE);
	}
	else if (p_nType == MB_CANCELTRYCONTINUE) {
		dlg.setSuspendMode(TRUE);
	}
	else {
		dlg.setCancelMode(FALSE);
	}

	if (p_nType | MB_ICONERROR) {
		dlg.setAlertIcon(IDB_PNG_ICON_ERROR);
	}
	else if (p_nType | MB_ICONINFORMATION) {
		dlg.setAlertIcon(IDB_PNG_ICON_DANGER);
	}
	else {
		dlg.setAlertIcon(IDB_PNG_ICON_WARNING);
	}

	int nRet = dlg.DoModal();
	return nRet;
}
// CKAgentQRApp initialization

BOOL CKAgentQRApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR m_gdiplusToken;
	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, nullptr);

	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager* pShellManager = new CShellManager;

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
	if (strCmdLine.Find(L"--alert") >= 0) {
		GdiplusStartupInput m_gdiplusStartupInput;
		GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, nullptr);

		if (KMessageBox(L"Are you sure run agent script", MB_OKCANCEL) == IDOK){
			ExitProcess(500);
		}
		else {
			ExitProcess(0);
		}
	}


	while (TRUE) {
		if (ClaProcess::FindProcessID(L"explorer.exe") <= 0) {
			Sleep(2000);
			continue;
		}
		break;
	}

	Sleep(8000);


	CKAgentQRDlg dlg;
	//m_pMainWnd = &dlg;
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

