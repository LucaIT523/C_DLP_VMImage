#pragma once

#include "KWnd.h"

// KStaticPng
class KStaticText : public KWnd
{
	DECLARE_DYNAMIC(KStaticText)

public:
	KStaticText();
	virtual ~KStaticText();

	void setFont(const wchar_t* p_szFace, int w, BOOL bold, COLORREF c, int linespace = 0) {
		wcscpy_s(_wszFont, p_szFace);
		_w = w; _bold = bold; _c = c;
		_linespace = linespace;
	};
	void setAlign(int p_nAlign, int p_nAlignV = 1) { _align = p_nAlign; _alignV = p_nAlignV; };
	wchar_t _wszFont[64]; int _w; BOOL _bold;
	int _align, _alignV, _linespace; COLORREF _c;

	virtual void Render(Gdiplus::Graphics& g);

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	virtual void UpdateState();
	afx_msg LRESULT OnSetText(WPARAM, LPARAM);
};


