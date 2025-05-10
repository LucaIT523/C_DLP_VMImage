#pragma once

#include <gdiplus.h>

using namespace Gdiplus;

class ClaStaticFont : public CStatic
{
public:
    ClaStaticFont();
    virtual ~ClaStaticFont();

public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();

public:
    void SetFontColor(COLORREF color);
    void SetFontStyle(const CString& fontFace, int fontSize, BOOL isBold, int Align, int vAlign = 1);
    void SetBackgroundColor(COLORREF color, COLORREF colorBorder);

    afx_msg LRESULT OnSetText(WPARAM, LPARAM);
    afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private:
    CBitmap m_Bmp;

private:
    CFont m_font;
    COLORREF m_fontColor;
    int align;
    int valign;
    COLORREF m_bgColor;
    COLORREF m_bdColor;
};