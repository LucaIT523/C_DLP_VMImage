#pragma once
#include "afxdialogex.h"
#include "KDialog.h"
#include "KStaticPng.h"

// ClaDlgBound dialog

class ClaDlgBound : public KDialog
{
public:
	ClaDlgBound(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaDlgBound();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_BOUND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual KDialog* CreateStandardDialog() {
		return NULL;
	}
	;
	virtual void OnInitChildrenWnds();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	KStaticPng m_stcIcon;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
};
