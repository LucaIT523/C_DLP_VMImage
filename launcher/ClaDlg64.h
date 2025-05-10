#pragma once
#include "afxdialogex.h"


// ClaDlg64 dialog

class ClaDlg64 : public CDialog
{
	DECLARE_DYNAMIC(ClaDlg64)

public:
	ClaDlg64(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaDlg64();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_64 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};
