// KenanConn.h : main header file for the KenanConn DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CKenanConnApp
// See KenanConn.cpp for the implementation of this class
//

class CKenanConnApp : public CWinApp
{
public:
	CKenanConnApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
