
// launcher.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "KGlobal.h"
#include "KVmMgr.h"
#include "ClaLogMgr.h"


// ClauncherApp:
// See launcher.cpp for the implementation of this class
//

class ClauncherApp : public CWinApp
{
public:
	ClauncherApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

protected:
	unsigned int checkEnv();

public:
	CString m_strServer;
};

extern ClauncherApp theApp;
extern KGlobal g_kenan;
extern KVmMgr *g_pVmMgr;
extern ClaLogMgr* g_pLog;
extern CString g_strIP;
extern CString g_strGlobalIP;
extern int g_nPeriod;

#define COLOR_BACKGROUND	RGB(112, 140, 220)
#define COLOR_BORDER		RGB(78, 106, 186)
#define COLOR_TEXT			RGB(0, 40, 60)
#define COLOR_HYPER			RGB(80, 80, 180)
#define COLOR_BUTTON		RGB(60, 90, 130)

#define FONT_FACE			L"Times New Roman"
#define FONT_SIZE_TITLE		32
#define FONT_SIZE_SUBTITLE	36
#define FONT_SIZE_TEXT		24


#define GD_COLOR_DLG_BG		RGB(222, 230, 245)
#define GD_COLOR_DLG_BD		RGB(144, 160, 190)
#define GD_COLOR_DLG_TITLE	GD_COLOR_GROUP_BG
#define GD_COLOR_GROUP_BG	RGB(241, 247, 254)
#define GD_COLOR_GROUP		RGB(255, 255, 255)
#define GD_COLOR_LABEL		RGB(80, 80, 80)
#define GD_COLOR_BTN_BG		RGB(62, 70, 132)
#define GD_COLOR_BTN_BG1	RGB(78, 106, 186)
#define GD_COLOR_BTN_TXT	RGB(255, 255, 255)

#define GD_COLOR_LIST1		RGB(222, 230, 245)
#define GD_COLOR_LIST2		RGB(241, 247, 254)


#define GD_FONT_FACE		L"Arial"
#define GD_FONT_SIZE_TEXT	18
#define GD_FONT_SIZE_BTN	22
#define GD_FONT_SIZE_EDIT	24
#define GD_FONT_SIZE_EDIT_OTP	36
#define GD_FONT_SIZE_TITLE	32
