
// KAgentQRDlg.h : header file
//

#pragma once

#include <string>
#include "qrcodegen.hpp"
#include "ClaTrayIconMgr.h"

using namespace qrcodegen;

// CKAgentQRDlg dialog
class CKAgentQRDlg : public CDialog
{
// Construction
public:
	CKAgentQRDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KAGENTQR_DIALOG };
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
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	
	CBitmap m_qrBitmap; // Bitmap to hold the QR code
	std::string _getQrText();
	void GenerateQRCode(const std::string& text);

	ClaTrayIconMgr _trayMgr;

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	void OnTrayOpen();
	void OnTrayExit();
	void OnTrayAbout();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
};
