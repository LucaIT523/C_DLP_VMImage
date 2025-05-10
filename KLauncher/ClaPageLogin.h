#pragma once
#include "afxdialogex.h"
#include "KDialog.h"
#include "KStaticPng.h"
#include "KStaticText.h"
#include "KEdit.h"
#include "KCheckbox.h"
#include "KButtonPng.h"

// ClaPageLogin dialog

class ClaPageLogin : public KDialog
{
public:
	ClaPageLogin(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaPageLogin();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE_LOGIN };
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
	KEdit m_edtMID;
	KEdit m_edtMail;
	KEdit m_edtPwd;
	KEdit m_edtIP;
	KCheckbox m_chkRemember;
	KButtonPng m_btnSignin;
	afx_msg void OnBnClickedBtnLogin();
	virtual void OnShowPage();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	CString m_strMID;
	CString m_strMail;
	CString m_strPwd;
	CString m_strIP;
	int m_nPort;
	BOOL m_bRemember;

	void _saveCredential();
	void _loadCredential();
};
