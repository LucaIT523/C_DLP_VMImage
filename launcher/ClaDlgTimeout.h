#pragma once
#include "afxdialogex.h"


// ClaDlgTimeout dialog

class ClaDlgTimeout : public CDialog
{
	DECLARE_DYNAMIC(ClaDlgTimeout)

public:
	ClaDlgTimeout(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaDlgTimeout();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_TIMEOUT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

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
};
