#pragma once
#include "afxdialogex.h"


// ClaDlgConfig dialog

class ClaDlgConfig : public CDialog
{
	DECLARE_DYNAMIC(ClaDlgConfig)

public:
	ClaDlgConfig(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaDlgConfig();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CONFIG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strServer;
	CString m_strPort;
	afx_msg void OnBnClickedOk();
};
