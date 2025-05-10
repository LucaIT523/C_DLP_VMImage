
// KLauncherDlg.h : header file
//

#pragma once
#include "KDialog.h"
#include "KButtonPng.h"
#include "KC_common.h"
#include "KStaticText.h"
#include "KRadioPng.h"
#include "ClaTrayIconMgr.h"


// CKLauncherDlg dialog
class ClaDlgTrial : public KDialog
{
	// Construction
public:
	ClaDlgTrial(UINT p_nBgID, CWnd* pParent = nullptr);	// standard constructor

	enum { IDD = IDD_KLAUNCHER_TRIAL_DIALOG };

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

	KDialog* _pCurPage;

protected:

public:
	KButtonPng m_btnClose;
	afx_msg void OnDestroy();
	virtual void OnCancel();
	//
	//	last validate state
	//
	CString _strValidateMessage;
	int _nValidateStatus;
	int _nValidateSpeed;
	afx_msg void OnBnClickedBtnMin();

	int m_current;
	CString m_description_list[3];
	KStaticText m_description;
	KButtonPng m_btn_skip;
	KButtonPng m_btn_next;
	KStaticText m_trial_img;
	KRadioPng m_btn_trial;
	afx_msg void OnBnClickedBtnNext();
	KRadioPng m_radio;
	KButtonPng m_btn_ok;
	afx_msg void OnBnClickedBtnFinish();
	afx_msg void OnBnClickedBtnSkip();
};