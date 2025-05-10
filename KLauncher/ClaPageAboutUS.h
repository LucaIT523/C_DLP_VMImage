#pragma once
#include "afxdialogex.h"
#include "KDialog.h"
#include "KButtonPng.h"
#include "KStaticPng.h"
#include "KC_common.h"
// ClaPageAboutUS dialog

class ClaPageAboutUS : public KDialog
{
public:
	ClaPageAboutUS(CWnd* pParent = nullptr);
	virtual ~ClaPageAboutUS();
	virtual KDialog* CreateStandardDialog() {
		return NULL;
	};
	virtual void OnInitChildrenWnds();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE_ABOUT_US };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	KStaticPng m_pngLogo;
	KButtonPng m_btnBack;
	ClaKcGroup _group;
	afx_msg void OnBnClickedBtnBack();
	void setGroup(ClaKcGroup* p_pGroup) {
		_group = p_pGroup[0];
	}
	KStaticPng m_background;
};
