#pragma once
#include "afxdialogex.h"


// ClaDlgDownload dialog

class ClaDlgDownload : public CDialog
{
	DECLARE_DYNAMIC(ClaDlgDownload)

public:
	ClaDlgDownload(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaDlgDownload();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DOWNLOAD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int _execute(const wchar_t* p_szEXE, const wchar_t* p_pszCommandParam);
	void download_unzip();

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedCancel();

	int m_nID;
	CString m_strName;
	CString m_strTitle;
	CString m_strPercent;
	CProgressCtrl m_pgsDownload;

};
