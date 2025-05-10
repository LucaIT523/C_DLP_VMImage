#pragma once
#include "afxdialogex.h"


// ClaDlgTest dialog

class ClaDlgTest : public CDialogEx
{
	DECLARE_DYNAMIC(ClaDlgTest)

public:
	ClaDlgTest(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaDlgTest();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_TEST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strTest;
	CString m_strPass;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
};
