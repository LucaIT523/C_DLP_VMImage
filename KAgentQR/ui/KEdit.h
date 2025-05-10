#pragma once

#include "KWnd.h"

// KStaticPng
class KEdit : public KWnd
{
	DECLARE_DYNAMIC(KEdit)

public:
	KEdit();
	virtual ~KEdit();

	bool LoadImageK(UINT imageID[2], LPCTSTR lpszResType = _T("PNG"), HINSTANCE hInstance = NULL);
	bool SetIconImage(UINT imageID, LPCTSTR lpszResType = _T("PNG"), HINSTANCE hInstance = NULL);
	bool SetRightIcon(UINT imageID, LPCTSTR lpszResType = _T("PNG"), HINSTANCE hInstance = NULL, UINT p_nMSGID = 0);
	void setFont(const wchar_t* p_szFace, int w, BOOL bold, COLORREF c, COLORREF c1) {
		wcscpy_s(_wszFont, p_szFace);
		_w = w; _bold = bold; _c = c; _c1 = c1;
	};
	void setAlign(int p_nAlign) { _align = p_nAlign; };
	void setPasswordChar(wchar_t p_wzPwd) {
		_wzPwd = p_wzPwd;
	}

	virtual void Render(Gdiplus::Graphics& g);

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	virtual void UpdateState();
	afx_msg LRESULT OnSetText(WPARAM, LPARAM);

	Gdiplus::Image* m_pCurrImage;
	Gdiplus::Image* m_pIconImage;
	Gdiplus::Image* m_pImageBtn;
	wchar_t _wszFont[64]; int _w; BOOL _bold;
	int _align; COLORREF _c, _c1;
	BOOL _bFocus;
	BOOL _btn_ready;
	wchar_t _wzPwd;
	BOOL _bDownIcon;
	UINT _nMsgID;
public:
	int _nPaddingLeft;
	int _nPaddingLeft1;
	int _nPaddingRight;
	int _nPaddingTop;
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};


