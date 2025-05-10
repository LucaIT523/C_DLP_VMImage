#pragma once
#include "afxdialogex.h"
#include "ClaDialogPng.h"
#include "ClaStaticPng.h"
#include "ClaStaticFont.h"
#include "ClaButtonDraw.h"
#include "ClaCheckDraw.h"
#include "ClaEditCtrl.h"
// ClaDlgLogin dialog

class ClaDlgLogin : public ClaDialogPng
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
	void _init_ui();
	void _init_ip();
	void _read();
	void _write();

private:
	CString _strServer;
	CString _strPort;
public:
	afx_msg void OnBnClickedBtnSetting();
	afx_msg void OnBnClickedOk();

	void _savePassword();
	void _loadPassword();

	CString m_strUser;
	CString m_strPwd;
	CString m_strMachineID;
	BOOL m_bRemember;

	ClaStaticFont m_grpGB;
	ClaStaticPng m_stcIcon;
	ClaStaticFont m_stcMid;
	ClaStaticFont m_stcEmail;
	ClaStaticFont m_stcPwd;
	ClaStaticFont m_stcClick;
	ClaButtonDraw m_btnOK;
	ClaCheckDraw m_chkRemember;
	ClaButtonDraw m_btnCancel;
	ClaStaticFont m_grpMachine;
	ClaStaticFont m_grpMail;
	ClaStaticFont m_grpPwd;
	afx_msg void OnStnClickedStcClick();
	ClaStaticFont m_grpIP;
	ClaStaticFont m_stcIP;
	CComboBox m_cmbIP;
	ClaStaticFont m_stcTitle;
	ClaStaticFont m_stcSubTitle;
	ClaEditCtrl m_edtMachine;
	ClaEditCtrl m_edtMail;
	ClaEditCtrl m_edtPwd;
	ClaButtonDraw m_btnMin;
	ClaButtonDraw m_btnConfig;
	afx_msg void OnBnClickedBtnMin();
	ClaButtonDraw m_btnEye;
	ClaEditCtrl m_edtServer;
	CString m_strServer;
	ClaStaticFont m_grpServer;

};
