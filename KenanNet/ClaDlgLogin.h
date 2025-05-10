#pragma once
#include "afxdialogex.h"


// ClaDlgLogin dialog

class ClaDlgLogin : public CDialog
{
	DECLARE_DYNAMIC(ClaDlgLogin)

public:
	ClaDlgLogin(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaDlgLogin();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_LOGIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

public:
	void _read();
	void _write();

private:
	CString _strServer;
	CString _strPort;
public:
	afx_msg void OnBnClickedBtnSetting();
	afx_msg void OnBnClickedOk();
	CString m_strUser;
	CString m_strPwd;
};
