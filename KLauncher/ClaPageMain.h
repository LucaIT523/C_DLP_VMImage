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
#include "KStaticMenu.h"

// ClaPageMain dialog

class ClaPageMain : public KDialog
{
public:
	ClaPageMain(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaPageMain();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE_MAIN};
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
	KStaticPng m_stcConnected;
	KListCtrl m_lstGroup;
	KStaticPng m_grpPanel;
	KStaticText m_stcTitle3;
	KStaticText m_stcInfo1;
	KStaticText m_stcInfo2;
	KEdit m_edtPath;
	KStaticText m_stcInfo3;
	KButtonPng m_btnRefresh;
	KProgress m_pgsPercent;
	KStaticText m_stcPercent;
	KButtonPng m_btnPause;
	KButtonPng m_btnDownload;
	KButtonPng m_btnStart;
	KButtonPng m_btnPolicy;
	afx_msg void OnBnClickedBtnPolicy();
	virtual void OnShowPage();
	void _download_group_list();
	void _show_group_list();
	void _check_update_version();
	int _download_update_version();
	int _download_agent_update();

protected:
	int _nGroupID;
	int _nGroupIdx;
	int _nImageID;
	int _nImageIdx;
	ClaKcPolicy m_policy;
	CString m_strViolationResult;
	ClaKcGroup m_group;
	BOOL m_bStarting;
	void OnGroupListSelChanged(int p_nSel);
	afx_msg void OnBnClickedBtnRefresh();
	void onAfterVmLaunched(int p_result);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	CString m_strInfo1;
	CString m_strInfo2;
	CString m_strPath;
	CString m_strInfo3;
	unsigned int post_to_agent(const char* p_szIP, BOOL p_bIP, BOOL p_bPORT, BOOL p_bViolation = FALSE, BOOL p_bAgentScript = FALSE);
	std::string getScripts();
	void report_agent_script(const char* p_szResultScript);
	void set_environment();
	void policy_update();
	void check_update();
	int _nReason;
	void onAfterStopVM(BOOL p_bStop = TRUE, int reason = 0);
	void onAfterVMStoped();
	afx_msg void OnBnClickedBtnStart();
	CString m_strPercent;
	int m_nDownGID;
	int m_nDownIID;

	int _execute(const wchar_t* p_szEXE, const wchar_t* p_pszCommandParam);
	void download_unzip();
	void SetButtonImage(KButtonPng& btn, UINT imgID, UINT imgID_Hover);
	BOOL m_bDownloadContinue;
	afx_msg void OnBnClickedBtnDownload();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	KStaticText m_stcSpeed;
	CString m_strSpeed;

	BOOL _checkReqirement(ClaKcVmImage* image);
	KStaticPng m_netSpeedBg;
	KStaticCombo m_cmbMenu;
	BOOL _bPausedDownload;
	afx_msg void OnBnClickedBtnPause();
	KStaticText m_stcDownName;
	CString m_strDownName;
	CString m_strRate;
	KStaticText m_stcRate;
	KStaticMenu m_stcPopup;
};
