#pragma once
#include "afxdialogex.h"
#include "KDialog.h"
#include "KButtonPng.h"
#include "KStaticText.h"

// ClaDlgTimeout dialog

class ClaDlgTimeout : public KDialog
{
public:
	ClaDlgTimeout(UINT p_nBgID, CWnd* pParent = nullptr);   // standard constructor
	enum { IDD = IDD_DLG_TIMEOUT };
	virtual ~ClaDlgTimeout();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_TIMEOUT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual KDialog* CreateStandardDialog();
	virtual void OnInitChildrenWnds();
	DECLARE_MESSAGE_MAP()
public:
	int m_nSecond;
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CString m_strTitle;
	CString m_strData;
	int m_nMax;
	BOOL m_bIDOK;

	CString m_strMessage;
	afx_msg void OnBnClickedOk();
	afx_msg void OnDestroy();
	KButtonPng m_alert_btn_ok;
	KButtonPng m_alert_btn_cancel;
	KButtonPng m_btnClose;
	afx_msg void OnBnClickedBtnExit();
	KStaticText m_strDescription;
	KStaticText m_stcMessage1;
	CString m_strMessage1;
};
