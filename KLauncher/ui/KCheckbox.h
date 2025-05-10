#pragma once

#include "KWnd.h"

// KCheckbox
class KCheckbox : public KWnd
{
	DECLARE_DYNAMIC(KCheckbox)

public:
	KCheckbox();
	virtual ~KCheckbox();
	
	bool LoadImageList(UINT imageID[2], LPCTSTR lpszResType = _T("PNG"), HINSTANCE hInstance = NULL);

	virtual void Render(Gdiplus::Graphics& g);

	void setFont(const wchar_t* p_szFace, int w, BOOL bold, COLORREF c) {
		wcscpy_s(_wszFont, p_szFace);
		_w = w; _bold = bold; _c = c;
	};
	void setAlign(int p_nAlign) { _align = p_nAlign; };

	wchar_t _wszFont[64]; int _w; BOOL _bold;
	int _align; COLORREF _c;



protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	virtual void UpdateState();

	Gdiplus::Image* m_pCurrImage;
	bool m_bMouseOver;
	bool m_bPressed;
	bool m_bSelected;
	bool m_bTracking;
	int m_nPos;
	int m_nPosOld;

public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};


