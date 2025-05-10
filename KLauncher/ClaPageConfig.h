#pragma once
#include "afxdialogex.h"
#include "KDialog.h"
#include "KStaticPng.h"
#include "KStaticText.h"
#include "KEdit.h"
#include "KCheckbox.h"
#include "KButtonPng.h"

// ClaPageConfig dialog

class ClaPageConfig : public KDialog
{
public:
	ClaPageConfig(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaPageConfig();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE_CONFIG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual KDialog* CreateStandardDialog() {
		return NULL;
	};
	virtual void OnInitChildrenWnds() ;

public:
	virtual BOOL OnInitDialog();
	KStaticPng m_stcLogo;
	KStaticText	m_stcTitle;
	KStaticText m_stcTitle2;
	KStaticText m_stcTitle3;
	KStaticPng m_stcPanel;

	KEdit m_edtIp;
	KEdit m_edtPort;
	KButtonPng m_btnBack;
	KButtonPng m_btnSave;
	afx_msg void OnBnClickedBtnBack();
	afx_msg void OnBnClickedBtnSave();
	CString m_strIP;
	CString m_strPort;

	virtual void OnShowPage();
};
