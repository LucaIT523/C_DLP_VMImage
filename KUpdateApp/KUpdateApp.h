
// KUpdateApp.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include <Windows.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <iostream>
#include <codecvt>
// CKUpdateAppApp:
// See KUpdateApp.cpp for the implementation of this class
//

class CKUpdateAppApp : public CWinApp
{
public:
	CKUpdateAppApp();

// Overrides
public:
	virtual BOOL InitInstance();

	void updateFilesandRun();
	BOOL _deleteFiles(const wchar_t* p_wszPath);
	BOOL _copyFiles(const wchar_t* p_wszSrc, const wchar_t* p_wszDst);

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CKUpdateAppApp theApp;
