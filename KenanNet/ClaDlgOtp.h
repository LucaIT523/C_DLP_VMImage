#pragma once
#include "afxdialogex.h"


// ClaDlgOtp dialog

class ClaDlgOtp : public CDialog
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

	void _read();
	CString _strServer;
	CString _strPort;
};
