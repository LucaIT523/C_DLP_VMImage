
// KLauncher.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "ClaLogMgr.h"
#include "KVmMgr.h"
#include "ClaFileMap.h"

// CKLauncherApp:
// See KLauncher.cpp for the implementation of this class
//

class CKLauncherApp : public CWinApp
{
public:
	CKLauncherApp();

public:
	void _init();
	void _uninit();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

public:
	BOOL _checkUpdate();
	BOOL _checkFiles();
	BOOL _checkEnv();
	BOOL relaunchClient(BOOL p_bMessage);

public:
	ClaFileMap _map;
};

UINT KMessageBox(const wchar_t* p_wszMessage, UINT p_nType = MB_OK);

extern CKLauncherApp theApp;
extern ClaLogMgr* g_pLog;
extern KVmMgr* g_pVmMgr;
extern CString g_strLocalIP;
extern CString g_strGlobalIP;
extern CString g_strMachineID;
extern int g_nPolicyUpdatePeriod;
extern int g_checkUpdateInterval;
extern HANDLE g_hProcNodeClient;
extern CString g_strSessionID;
extern CString g_strUserID;
