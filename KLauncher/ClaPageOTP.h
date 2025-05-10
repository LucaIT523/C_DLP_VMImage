#pragma once
#include "afxdialogex.h"
#include "KDialog.h"
#include "KStaticPng.h"
#include "KStaticText.h"
#include "KEdit.h"
#include "KCheckbox.h"
#include "KButtonPng.h"
#include "KEditPin.h"

// ClaPageOTP dialog

class ClaPageOTP : public KDialog
{
public:
	ClaPageOTP(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaPageOTP();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE_OTP};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual KDialog* CreateStandardDialog() {
		return NULL;
	};
	virtual void OnInitChildrenWnds() ;

public:
	virtual BOOL OnInitDialog();
	KStaticPng m_stcLogo;
	KStaticText	m_stcTitle;
	KStaticText m_stcTitle2;
	KStaticText m_stcTitle3;
	KStaticPng m_stcPanel;

	KEditPin m_edtPin1;
	KEditPin m_edtPin2;
	KEditPin m_edtPin3;
	KEditPin m_edtPin4;
	KEditPin m_edtPin5;
	KEditPin m_edtPin6;
	KStaticText m_stcResend;
	KButtonPng m_btnBack;
	KButtonPng m_btnLogin;
	afx_msg void OnBnClickedBtnBack();
	afx_msg void OnEnChangeEdtPin1();
	afx_msg void OnEnChangeEdtPin2();
	afx_msg void OnEnChangeEdtPin3();
	afx_msg void OnEnChangeEdtPin4();
	afx_msg void OnEnChangeEdtPin5();
	afx_msg void OnEnChangeEdtPin6();
	
	CString m_strPin1;
	CString m_strPin2;
	CString m_strPin3;
	CString m_strPin4;
	CString m_strPin5;
	CString m_strPin6;
	afx_msg void OnBnClickedBtnLogin();
	virtual void OnShowPage();
};
