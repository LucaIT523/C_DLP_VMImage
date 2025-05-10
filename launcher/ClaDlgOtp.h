#pragma once
#include "afxdialogex.h"
#include "ClaButtonDraw.h"
#include "ClaDialogPng.h"
#include "ClaStaticFont.h"
#include "ClaStaticPng.h"
#include "ClaEditCtrl.h"

// ClaDlgOtp dialog

class ClaDlgOtp : public ClaDialogPng
{
	DECLARE_DYNAMIC(ClaDlgOtp)

public:
	ClaDlgOtp(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaDlgOtp();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_OTP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strNum1;
	CString m_strNum2;
	CString m_strNum3;
	CString m_strNum4;
	CString m_strNum5;
	CString m_strNum6;
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEdtNum1();
	afx_msg void OnEnChangeEdtNum2();
	afx_msg void OnEnChangeEdtNum3();
	afx_msg void OnEnChangeEdtNum4();
	afx_msg void OnEnChangeEdtNum5();
	afx_msg void OnEnChangeEdtNum6();
	void _enableUI();

	void _read();
	CString _strServer;
	CString _strPort;


	ClaStaticFont m_grpBG;
	ClaStaticPng m_stcLogo;
	ClaStaticFont m_grpOTP;
	ClaStaticFont m_stcOTP;
	ClaStaticFont m_stcResend;
	ClaButtonDraw m_btnOK;
	ClaButtonDraw m_btnCancel;
	ClaStaticFont m_stcTitle;
	ClaStaticFont m_stcSubTitle;
	ClaEditCtrl m_edtNum1;
	ClaEditCtrl m_edtNum2;
	ClaEditCtrl m_edtNum3;
	ClaEditCtrl m_edtNum4;
	ClaEditCtrl m_edtNum5;
	ClaEditCtrl m_edtNum6;
	ClaButtonDraw m_btnMin;
	afx_msg void OnBnClickedBtnMin();
	ClaButtonDraw m_btnBack;
	afx_msg void OnBnClickedBtnBack();

	BOOL m_bBack;
	afx_msg void OnStnClickedStcResend();

	CString m_strUser, m_strPwd;
};
