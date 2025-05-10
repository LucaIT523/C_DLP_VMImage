#pragma once

#include "KWnd.h"
#include <vector>
#include <string>
// KStaticPng
class KStaticCombo : public KWnd
{
	DECLARE_DYNAMIC(KStaticCombo)

public:
	KStaticCombo();
	virtual ~KStaticCombo();
	typedef std::vector<Gdiplus::Image*> ImageList;
	void DrawText_(Gdiplus::Graphics& g, const wchar_t* text, Gdiplus::RectF& rt, BOOL p_bHead);
	ImageList m_imageList;
	bool LoadImageList(UINT imageID[TWS_COMBO_NUM], LPCTSTR lpszResType = _T("PNG"), HINSTANCE hInstance = NULL, BOOL p_bResize = TRUE);
	void _setWindowPos();
	void setFont(const wchar_t* p_szFace, int w, BOOL bold, COLORREF c, int linespace = 0) {
		wcscpy_s(_wszFont, p_szFace);
		_w = w; _bold = bold; _c = c;
		_linespace = linespace;
	};
	wchar_t _wszFont[64]; int _w; BOOL _bold;
	int _align, _alignV, _linespace; COLORREF _c;
	
	void addItem(const wchar_t* p_wszItem);
	void setCurSel(int p_nItem);
	virtual void Render(Gdiplus::Graphics& g);

	BOOL m_isActive;
	int m_paddingLeft;
	int m_paddingRight;

	int m_nCurSel;
	int getCurSel() { return m_nCurSel; };
	int m_nHover;
protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	virtual void UpdateState();
	afx_msg LRESULT OnSetText(WPARAM, LPARAM);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	UINT _msgID;
	void setMsgID(UINT p_msg) {
		_msgID = p_msg;
	}

	CStringArray _items;
	CArray<RECT, RECT&> _rtItems;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);

};


