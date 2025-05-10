#pragma once

#include <gdiplus.h>

using namespace Gdiplus;

class ClaCheckDraw : public CButton
{
public:
    ClaCheckDraw();
    virtual ~ClaCheckDraw();

public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();

public:
    void setColor(COLORREF bg, COLORREF bd, COLORREF txt);
    void setFont(const CString& fontFace, int fontSize, BOOL isBold, int p_nAlign);
    void setImage(UINT p_nCheck, UINT p_nUncheck);

private:
    CFont m_font;
    int align;
    COLORREF m_fontColor;
    COLORREF m_bgColor;
    COLORREF m_bdColor;
    Bitmap* m_pCheck, * m_pUncheck;

    BOOL bPress, bHover;
public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseHover(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();
    afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};