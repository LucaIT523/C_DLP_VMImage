
// KLauncherDlg.h : header file
//

#pragma once
#include "KDialog.h"
#include "ClaPageLogin.h"
#include "ClaPageConfig.h"
#include "ClaPageOTP.h"
#include "ClaPageMain.h"
#include "ClaPagePortlist.h"
#include "ClaPageAboutUS.h"
#include "KButtonPng.h"
#include "KC_common.h"
#include "ClaTrayIconMgr.h"


// CKLauncherDlg dialog
class CKLauncherDlg : public KDialog
{
// Construction
public:
	CKLauncherDlg(UINT p_nBgID, CWnd* pParent = nullptr);	// standard constructor

	enum { IDD = IDD_KLAUNCHER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	virtual KDialog* CreateStandardDialog();
	virtual void OnInitChildrenWnds();
	void ShowConfigPage();
	void ShowLoginPage();
	void ShowOtpPage();
	void ShowMainPage(BOOL p_bCallback = TRUE);
	void ShowPortPage(ClaKcGroup* pGroup);
	void ShowAboutPage();
	KDialog* _pCurPage;

protected:
	ClaPageLogin* _pPageLogin;
	ClaPageConfig* _pPageConfig;
	ClaPageOTP* _pPageOTP;
	ClaPageMain* _pPageMain;
	ClaPagePortlist* _pPagePort;
	ClaPageAboutUS* _pPageAboutUs;
public:
	KButtonPng m_btnMin;
	KButtonPng m_btnClose;
	afx_msg void OnDestroy();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	//
	//	last validate state
	//
	CString _strValidateMessage;
	int _nValidateStatus;
	int _nValidateSpeed;
	CString _strRate;
	afx_msg void OnBnClickedBtnMin();
	void OnTrayOpen();
	void OnTrayExit();

	BOOL m_bMinimize;
	ClaTrayIconMgr _trayMgr;

};

extern CKLauncherDlg* g_pMain;
BOOL CALLBACK_VALIDATE(void* p, int result, const char* p_szMessage, int p_nSpeed, char* p_szRate);