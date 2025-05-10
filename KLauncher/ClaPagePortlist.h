#pragma once
#include "afxdialogex.h"
#include "KDialog.h"
#include "KStaticPng.h"
#include "KStaticText.h"
#include "KEdit.h"
#include "KCheckbox.h"
#include "KButtonPng.h"
#include "KProgress.h"
#include "KListCtrl.h"
#include "KC_common.h"
#include "KStaticCombo.h"

// ClaPagePortlist dialog

class ClaPagePortlist : public KDialog
{
public:
	ClaPagePortlist(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaPagePortlist();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE_PORT};
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
	KStaticPng m_stcTitle;
	KListCtrl m_lstPort;
	KStaticText m_stcTitle3;
	KButtonPng m_btnOK;
	ClaKcGroup _group;
	afx_msg void OnBnClickedBtnOk();
	virtual void OnShowPage();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void setGroup(ClaKcGroup* p_pGroup) {
		_group = p_pGroup[0];
	}
	KStaticCombo m_cmbMenu;
};
