// KenanHelperVM.cpp : Defines the initialization routines for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "KenanHelperVM.h"
#include "KenanHoos.h"
#include "ClaRegMgr.h"
#include "KGlobal.h"
#include "ClaProcess.h"
#include "kncrypto.h"

#pragma comment(lib, "detours.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CKenanHelperVMApp
BOOL lv_bSecuProcess;

BEGIN_MESSAGE_MAP(CKenanHelperVMApp, CWinApp)
END_MESSAGE_MAP()


// CKenanHelperVMApp construction
CKenanHelperVMApp::CKenanHelperVMApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CKenanHelperVMApp object

CKenanHelperVMApp theApp;

HINSTANCE g_hInstance = NULL;
int g_nProcID = 0;
int g_nParentProcID = 0;
wchar_t g_wszAppPath[MAX_PATH];
BOOL g_isMgrProc = FALSE;
BOOL g_isSecuProc = FALSE;
BOOL g_isVmxProc = FALSE;
HWND g_hwndMain = NULL;
wchar_t g_wszVML[MAX_PATH];
wchar_t g_wszEncPwd[100];


// CKenanHelperVMApp initialization

BOOL CKenanHelperVMApp::InitInstance()
{
	CWinApp::InitInstance();

	g_hInstance = theApp.m_hInstance;
	GetModuleFileName(NULL, g_wszAppPath, MAX_PATH);
	_wcsupr_s(g_wszAppPath, MAX_PATH);
	g_nProcID = GetCurrentProcessId();
	g_nParentProcID = ClaProcess::GetParentProcessId(g_nProcID);

	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	reg.readStringW(L"Software\\Kenan", L"dvmx", g_wszVML, MAX_PATH);

	ClaKcConfig config;
	KGlobal::readConfig(&config);
	memset(g_wszEncPwd, 0, sizeof(g_wszEncPwd));
	wcscpy_s(g_wszEncPwd, 100, config._wszEncPwd);

	init_hooks();

	return TRUE;
}

int CKenanHelperVMApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	uninit_hooks();

	if (g_isSecuProc)
	{
		CString strVmx = g_wszVML;
		strVmx.Replace(L".vml", L".vmx");
		kn_encrypt_file_with_pwd(g_wszVML, strVmx, g_wszEncPwd, L"");
		DeleteFile(g_wszVML);
	}

	return CWinApp::ExitInstance();
}
