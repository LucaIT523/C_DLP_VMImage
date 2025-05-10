#pragma once
#include "afxdialogex.h"
#include "KDialog.h"
#include "KButtonPng.h"
#include "KStaticPng.h"
// ClaDlgAboutAgent dialog

class ClaDlgAboutAgent : public KDialog
{

public:
	ClaDlgAboutAgent(UINT p_nBgID, CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaDlgAboutAgent();
	virtual KDialog* CreateStandardDialog();
	virtual void OnInitChildrenWnds();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE_ABOUT_US };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	KButtonPng m_btnClose;
	KStaticPng m_pngLogo;
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
