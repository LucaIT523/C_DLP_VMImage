#pragma once

#include "KWnd.h"

// KButtonPng
class KButtonPng : public KWnd
{
	DECLARE_DYNAMIC(KButtonPng)

public:
	KButtonPng();
	virtual ~KButtonPng();
	
	bool LoadImageList(UINT imageID[TWS_BUTTON_NUM], LPCTSTR lpszResType = _T("PNG"), HINSTANCE hInstance = NULL, BOOL p_bResize = TRUE);

	virtual void Render(Gdiplus::Graphics& g);

	// for checkbox or radiobutton
	void SetChecked(bool b);
	bool IsChecked() const;

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	virtual void UpdateState();

	Gdiplus::Image* m_pCurrImage;
	bool m_bResize;
	bool m_bMouseOver;
	bool m_bPressed;
	bool m_bSelected;
	bool m_bTracking;
	int m_nPos;
	int m_nPosOld;
	BOOL m_bUpdated;

public:
	void setFont(const wchar_t* p_szFace, int w, BOOL bold, COLORREF c) {
		wcscpy_s(_wszFont, p_szFace);
		_w = w; _bold = bold; _c = c;
	};
	void setAlign(int p_nAlign) { _align = p_nAlign; };
	wchar_t _wszFont[64]; int _w; BOOL _bold;
	int _align; COLORREF _c;

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


