#pragma once

#include <gdiplus.h>

using namespace Gdiplus;

class ClaEditCtrl : public CEdit
{
public:
    ClaEditCtrl();
    virtual ~ClaEditCtrl();

public:
    DECLARE_MESSAGE_MAP()

public:
    void SetFontColor(COLORREF color);
    void SetFontStyle(const CString& fontFace, int fontSize, BOOL isBold, int Align);
    void SetBackgroundColor(COLORREF color, COLORREF colorBorder);

private:
    CFont _font;
    COLORREF m_fontColor;
    int align;
    COLORREF m_bgColor;
    COLORREF m_bdColor;
    CBrush _Brush;
public:
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};