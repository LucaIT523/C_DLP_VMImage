
// KenanNet.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CKenanNetApp:
// See KenanNet.cpp for the implementation of this class
//

class CKenanNetApp : public CWinApp
{
public:
	CKenanNetApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

public:
	unsigned int _auth();
	unsigned int _down_list();
	unsigned int _down_unzip();
	unsigned int _logout();
	unsigned int _otp();

};

extern CKenanNetApp theApp;
