#pragma once

#include "KWnd.h"

// KStaticPng
class KEditPin : public KWnd
{
	DECLARE_DYNAMIC(KEditPin)

public:
	KEditPin();
	virtual ~KEditPin();

	bool LoadImageK(UINT imageID[4], LPCTSTR lpszResType = _T("PNG"), HINSTANCE hInstance = NULL);

	virtual void Render(Gdiplus::Graphics& g);

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	virtual void UpdateState();
	afx_msg LRESULT OnSetText(WPARAM, LPARAM);

	Gdiplus::Image* m_pCurrImage;
	BOOL _bFocus;

public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};


