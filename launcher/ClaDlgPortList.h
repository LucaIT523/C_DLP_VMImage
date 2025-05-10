#pragma once
#include "afxdialogex.h"
#include "ClaListCtrl.h"
#include "KC_common.h"
#include "ClaDialogPng.h"
#include "ClaButtonDraw.h"
#include "ClaStaticFont.h"
#include "ClaStaticPng.h"

// ClaDlgPortList dialog

class ClaDlgPortList : public ClaDialogPng
{
	DECLARE_DYNAMIC(ClaDlgPortList)

public:
	ClaDlgPortList(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaDlgPortList();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_PORTLIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	ClaListCtrl m_lstPortList;

	ClaKcGroup m_group;
	afx_msg void OnBnClickedBtnRefresh();
	afx_msg void OnBnClickedCancel();
	ClaButtonDraw m_btnCancel;
	ClaButtonDraw m_btnMin;
	ClaStaticPng m_stcLogo;
	ClaStaticFont m_stcTitle;
	ClaStaticFont m_grpBG;
	ClaStaticFont m_stcSubTitle;
};
