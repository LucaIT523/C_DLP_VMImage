#pragma once
#include "afxdialogex.h"
#include "KStaticPng.h"
#include "KStaticText.h"
#include "KButtonPng.h"
#include "KDialog.h"

// ClaDlgAlert dialog

class ClaDlgAlert : public KDialog
{
public:
	//ClaDlgAlert(CWnd* pParent = nullptr);
	ClaDlgAlert(UINT p_nBgID, CWnd* pParent = nullptr);
	enum { IDD = IDD_DLG_ALERT	};
	// standard constructor
	virtual ~ClaDlgAlert();

	virtual KDialog* CreateStandardDialog();
	virtual void OnInitChildrenWnds();


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_ALERT };
#endif

protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	KStaticPng m_alert_icon;
	KStaticText m_alert_description;
	KButtonPng m_alert_btn_cancel;
	KButtonPng m_alert_btn_ok;
	KButtonPng m_alert_btn_ok2;

	LPCTSTR m_alertText;
	UINT m_imageID;
	BOOL m_isCancel;
	BOOL m_isSuspend;
	void setAlertText(LPCTSTR p_alertText);
	void setAlertIcon(UINT p_imageID);
	void setCancelMode(BOOL p_isCancel);
	void setSuspendMode(BOOL p_isSuspend);
	KButtonPng m_btnClose;
	KButtonPng m_alert_icon_btn;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnCancel3();
	afx_msg void OnBnClickedBtnSuspend3();
	afx_msg void OnBnClickedBtnStop3();
	KButtonPng m_btnSuspend;
	KButtonPng m_btnStop3;
	KButtonPng m_btnCancel3;

	UINT _msgID;
	void setMsgID(UINT p_msg) {
		_msgID = p_msg;
	}

	UINT m_returnCode;
};
