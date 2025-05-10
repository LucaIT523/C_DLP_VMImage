// KenanHelperVM.h : main header file for the KenanHelperVM DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CKenanHelperVMApp
// See KenanHelperVM.cpp for the implementation of this class
//

class CKenanHelperVMApp : public CWinApp
{
public:
	CKenanHelperVMApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern HINSTANCE g_hInstance;
extern int g_nProcID;
extern int g_nParentProcID;
extern wchar_t g_wszAppPath[MAX_PATH];
extern wchar_t g_wszVML[MAX_PATH];
extern BOOL g_isMgrProc;
extern BOOL g_isSecuProc;
extern BOOL g_isVmxProc;
extern HWND g_hwndMain;
