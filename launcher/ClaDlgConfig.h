#pragma once
#include "afxdialogex.h"
#include "ClaDialogPng.h"
#include "ClaStaticFont.h"
#include "ClaStaticPng.h"
#include "ClaButtonDraw.h"
#include "ClaEditCtrl.h"

// ClaDlgConfig dialog

class ClaDlgConfig : public ClaDialogPng
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

	ClaStaticFont m_grpBG;
	ClaStaticPng m_grpLogo;
	ClaStaticFont m_grpIP;
	ClaStaticFont m_stcIP;
	ClaStaticFont m_grpPort;
	ClaStaticFont m_stcPort;
	ClaButtonDraw m_btnOK;
	ClaButtonDraw m_btnCancel;
	ClaStaticFont m_stcTitle;
	ClaStaticFont m_stcSubTitle;
	ClaEditCtrl m_edtServer;
	ClaEditCtrl m_edtPort;
	ClaButtonDraw m_btnMin;
	afx_msg void OnBnClickedBtnMin();
};
