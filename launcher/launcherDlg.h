
// launcherDlg.h : header file
//

#pragma once

#include "ClaModelImage.h"
#include "ClaListCtrl.h"
#include "ClaButtonIcon.h"
#include "KC_common.h"
#include "TreeListCtrl.h"
#include "ClaDialogPng.h"

#include "ClaButtonDraw.h"
#include "ClaStaticFont.h"
#include "ClaStaticPng.h"
#include "ClaEditCtrl.h"
#include "ClaFileMap.h"

// ClauncherDlg dialog
class ClauncherDlg : public ClaDialogPng
{
// Construction
public:
	ClauncherDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LAUNCHER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CArray<ClaModelImage*> m_lstModelImage;

	ClaKcPolicy m_policy;

	ClaListCtrl m_lstItems;
	ClaButtonDraw m_btnInfo;
	CMFCButton m_btnSetting;
	ClaButtonDraw m_btnRefresh;

	void download_unzip();
	int _execute(const wchar_t* p_szEXE, const wchar_t* p_pszCommandParam);
	afx_msg void OnBnClickedBtnDown();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnNMClickLstItem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkLstItem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();

	CString m_strVMX, m_strPwd;
	CString m_strDownName;
	int m_nDownID;
	int m_nDownGID;
	int m_nDownIID;
	void tf_set_environment();
	void tf_update_policy();
	unsigned int post_to_agent(const char* p_szIP, BOOL p_bIP, BOOL p_bPORT);
	int _launchRouter();

protected:
	BOOL _bStarting;
	int _nGroupID, _nImageID;
	ClaKcGroup _curGroup;
	KC_SETTING _setting;
	void _init_ui();
	void _load();
	void _show_list();
	void _show_status();
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnBnClickedBtnInfo();
	afx_msg void OnBnClickedBtnSetting();
	afx_msg void OnBnClickedBtnRefresh();
	void _update_ui();

	unsigned long _lLastTime;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	ClaStaticPng m_stcIcon;
	ClaStaticFont m_stcTitle;
	ClaButtonDraw m_btnMin;
	ClaButtonDraw m_btnCancel;
	ClaStaticFont m_grpBG;
	ClaStaticFont m_stcSubTitle;
	CButton m_btnStop;
	ClaButtonDraw m_btnStart;
	ClaButtonDraw m_btnDownload;
	ClaStaticFont m_stcPanel;
	ClaStaticFont m_stcPanel2;

	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnMin();
	ClaStaticFont m_stcOS;
	ClaStaticFont m_stcSize;
	ClaStaticFont m_stcDesc;
	CString m_strOS;
	CString m_strSize;
	CString m_strDesc;
	ClaStaticFont m_stcStatus;
	CString m_strStatus;
	ClaStaticFont m_stcPath;
	CString m_strPath;
	CString m_strXPath;

	BOOL _bTimer;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	ClaEditCtrl m_edtXPath;
	CProgressCtrl m_pgsDown;
	ClaStaticFont m_stcDown;
	CString m_strDown;

	BOOL m_bSessionOut;
	ClaFileMap _map;


};
