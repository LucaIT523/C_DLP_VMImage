
// KLauncher.cpp : Defines the class behaviors for the application.
//
#include "vld.h"

#include "pch.h"
#include "framework.h"
#include "KLauncher.h"
#include "KLauncherDlg.h"
#include "resource.h"

#include "ClaPathMgr.h"
#include "ClaRegMgr.h"
#include "ClaProcess.h"
#include "ClaFwMgr.h"
#include "KC_common.h"
#include "KGlobal.h"
#include "ClaNetAdapter.h"
#include "ClaDlgTrial.h"
#include "ClaDlgAlert.h"
#include "ClaDlgTimeout.h"
#include "ClaMutex.h"

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKLauncherApp

BEGIN_MESSAGE_MAP(CKLauncherApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CKLauncherApp construction

CKLauncherApp::CKLauncherApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CKLauncherApp object

CKLauncherApp theApp;
ULONG_PTR m_gdiplusToken;
CShellManager* pShellManager;

/// <summary>
/// 
/// </summary>
ClaLogMgr* g_pLog = NULL;
KVmMgr* g_pVmMgr = NULL;
CString g_strLocalIP;
CString g_strGlobalIP;
CString g_strMachineID;
int g_nPolicyUpdatePeriod;
int g_checkUpdateInterval = 10;
HANDLE g_hProcNodeClient = NULL;
CString g_strSessionID;
CString g_strUserID;

void CKLauncherApp::_init()
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
	pShellManager = new CShellManager;

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

	GdiplusStartupInput m_gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, nullptr);

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
	}
}

void CKLauncherApp::_uninit()
{
	WSACleanup();

	GdiplusShutdown(m_gdiplusToken);

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
}

// CKLauncherApp initialization
BOOL CKLauncherApp::InitInstance()
{
	_init();
	////////////////////////////////////////////////////////////////////////////
	g_pLog = new ClaLogMgr();
	g_pLog->loadFromIni();

	g_pVmMgr = new KVmMgr();

	g_pLog->LogFmtW(LOG_LEVEL_INFO, L"Launcher v1.0.0.1 started.");

	if (ClaMutex::IsCreate(L"KenanLauncher")) {
		KMessageBox(L"Kenan Launcher is already running.");
		goto L_EXIT;
	}
	ClaMutex::Create(L"KenanLauncher");
	
	if (_checkUpdate() == TRUE) {
		goto L_EXIT;
	}

	if (_checkFiles() == FALSE) {
		goto L_EXIT;
	}

	if (_checkEnv() == FALSE) {
		goto L_EXIT;
	}

	{
		ClaRegMgr reg(HKEY_LOCAL_MACHINE);
		DWORD dwTip = 0;
		reg.readInt(L"Software\\Kenan", L"Tip", dwTip);
		if (dwTip == 0)
		{
			ClaDlgTrial dlg(IDB_PNG_BG_TRIAL);
			dlg.DoModal();
			reg.createKey(L"Software\\Kenan");
			reg.writeInt(L"Software\\Kenan", L"Tip", 1);
		}
	}
	{
		CKLauncherDlg dlg(IDB_PNG_BG);
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
	}
	

L_EXIT:

	if (g_hProcNodeClient != NULL) {
		TerminateProcess(g_hProcNodeClient, 0);
	}

	if (g_pVmMgr) {
		delete g_pVmMgr;
	}

	delete g_pLog;
	if (kc_uninit != NULL) {
		kc_uninit();
	}
	////////////////////////////////////////////////////////////////////////////
	_uninit();

	return FALSE;
}

//	return TRUE if go on running, FALSE if stop and update
BOOL CKLauncherApp::_checkUpdate()
{
	BOOL bNeedUpdate = FALSE;
	DWORD dwFlg = -1;
	CString strUpdatePath; strUpdatePath.Format(L"%s\\updates", ClaPathMgr::GetDP());
	CString strUpdateApp; strUpdateApp.Format(L"%s\\KUpdateApp.exe", strUpdatePath);

	unsigned int exitCode;

	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	if (reg.readInt(L"Software\\Kenan", L"update_flag", dwFlg) != 0) {
		goto L_EXIT;
	}

	ClaProcess::CreateAndWait(strUpdateApp, L"--check", strUpdatePath, &exitCode);
	if (exitCode != 0x8000) {
		goto L_EXIT;
	}
	
	if (KMessageBox(L"New version of launcher is ready to update. Do you want to update it now?", MB_OKCANCEL) == IDOK) {
		bNeedUpdate = TRUE;
		goto L_EXIT;
	}

	if (dwFlg == 0) {
		KMessageBox(L"You have to update launcher.", MB_OK);
		goto L_EXIT;
	}

L_EXIT:
	if (bNeedUpdate) {
		ClaProcess::CreateProcessEx(strUpdateApp, L"", strUpdatePath, NULL, NULL);
	}

	return bNeedUpdate || dwFlg==0;
}

BOOL CKLauncherApp::_checkFiles()
{
	wchar_t* wszFiles[] = {
		L"iconv.dll",
		L"kclient.exe",
		L"KenanConn.dll",
		L"KenanHelperVM32.dll",
		L"KenanHelperVM64.dll",
		L"Launcher64.exe",
		L"libcrypto-3.dll",
		L"libcurl.dll",
		L"libssl-3.dll",
		L"libxml2.dll",
		L"vix.dll",
		L"vixd.dll",
		L"zlib1.dll",
		NULL
	};

	CString strDir = ClaPathMgr::GetDP();
	int nIndex = 0;
	while (wszFiles[nIndex] != NULL) {
		CString strFile; strFile.Format(L"%s\\%s", strDir, wszFiles[nIndex]);
		if (GetFileAttributes(strFile) == INVALID_FILE_ATTRIBUTES) {
			KMessageBox(L"The security environment could not be initialized because some files are missing.");
			return FALSE;
		}
		nIndex++;
	}
	return TRUE;
}

BOOL CKLauncherApp::_checkEnv()
{
	//
	//	check the vmware running.
	//
	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	wchar_t wszPlayerPath[MAX_PATH]; memset(wszPlayerPath, 0, sizeof(wszPlayerPath));
	
	if (reg.readStringW(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\vmware.exe", L"", wszPlayerPath) != 0
	&& reg.readStringW(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\vmplayer.exe", L"", wszPlayerPath) != 0) {
		KMessageBox(L"VMware is not installed yet. Please install VMware and try again.");
		return FALSE;
	}

	if (GetFileAttributes(wszPlayerPath) == INVALID_FILE_ATTRIBUTES) {
		KMessageBox(L"Cannot find the vmware player.");
		return FALSE;
	}

	//
	//	load kenan connector dll
	//
	if (kc_init(L"127.0.0.1", 9999) != 0) {
		g_pLog->LogFmtW(LOG_LEVEL_WARNING, L"Launcher - failed to init kc_init");
		KMessageBox(L"Failed to initialize the kenan network connector.");
		return FALSE;
	}
#ifndef _DEBUG
	std::string globalip = kc_get_global_ip();
	g_strGlobalIP.Format(L"%S", globalip.c_str());
	if (g_strGlobalIP == L"188.43.33.249")
#endif//_DEBUG
		g_strGlobalIP = L"172.92.33.42";

	g_strMachineID = KGlobal::GetMachineID();

	g_strLocalIP = ClaNetAdapter::GetIP();
	if (g_strGlobalIP.IsEmpty()) {
		g_strGlobalIP = g_strLocalIP;
	}
	//
	//	add kenan fw rule
	//
	CString strPath; strPath.Format(L"%s\\kclient.exe", ClaPathMgr::GetDP());
	ClaFwMgr::DeleteRule(L"KenanClientRule", TRUE);
	ClaFwMgr::AddAppRule(L"KenanClientRule", L"Application rule for kenan client", strPath.GetBuffer(), TRUE);
	Sleep(500);

	void* pMem;
	_map.createMap(L"Global\\kpol", 100, &pMem);

	if (relaunchClient(TRUE) == FALSE) {
		return FALSE;
	}

	return TRUE;
}

BOOL CKLauncherApp::relaunchClient(BOOL p_bMessage)
{
	CString strPath; strPath.Format(L"%s\\kclient.exe", ClaPathMgr::GetDP());

	//
	//	lauch kclient.exe
	//
	STARTUPINFO si; memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	DWORD dwPID = ClaProcess::FindProcessID(L"kclient.exe");
	if (dwPID != 0) {
		TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID), 0);
	}

	CString strCmd; strCmd.Format(L"\"%s\" \"%s\" \"%s\" \"%s\"", strPath, g_strGlobalIP, g_strSessionID, g_strUserID);
	if (ClaProcess::CreateProcessEx(strPath, strCmd, ClaPathMgr::GetDP(),
		NULL, &g_hProcNodeClient, &si) != 0) {
		if (p_bMessage)
			KMessageBox(L"Some files were correupted.");
		return FALSE;
	}
	return TRUE;
}

BOOL lv_bInitSyncMessage = FALSE;
CRITICAL_SECTION sync_message;


UINT KMessageBox(const wchar_t* p_wszMessage, UINT p_nType) {
	if (lv_bInitSyncMessage == FALSE) {
		InitializeCriticalSection(&sync_message);
		lv_bInitSyncMessage = TRUE;
	}
	EnterCriticalSection(&sync_message);
	ClaDlgAlert dlg(IDB_PNG_BG_ALERT);

	dlg.setAlertText(p_wszMessage);
	if (p_nType == MB_OKCANCEL) {
		dlg.setCancelMode(TRUE);
	}
	else if (p_nType == MB_CANCELTRYCONTINUE) {
		dlg.setSuspendMode(TRUE);
	}
	else if (p_nType == MB_YESNOCANCEL) {
		dlg.setStartContinueMode(TRUE);
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
	LeaveCriticalSection(&sync_message);
	return nRet;
}