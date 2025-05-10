// BoundaryTestDll.h : main header file for the BoundaryTestDll DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CBoundaryTestDllApp
// See BoundaryTestDll.cpp for the implementation of this class
//

class CBoundaryTestDllApp : public CWinApp
{
public:
	CBoundaryTestDllApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};
