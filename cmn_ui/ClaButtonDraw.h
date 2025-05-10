#pragma once

#include <gdiplus.h>

using namespace Gdiplus;

typedef void(*CB_MOUSE)(void* p, BOOL p_bFlg);

class ClaButtonDraw : public CButton
{
public:
    ClaButtonDraw();
    virtual ~ClaButtonDraw();

public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();

public:
    void setColor(COLORREF bg, COLORREF bd, COLORREF txt);
    void setBgImage(UINT p_bgImage = NULL);
    void setFont(const CString& fontFace, int fontSize, BOOL isBold, int p_nAlign);
    void setPressingCB(CB_MOUSE cb, void* p) {
        _cbMouse = cb; _pParamMouse = p;
    };

private:
    CFont m_font;
    int align;
    COLORREF m_fontColor;
    COLORREF m_bgColor;
    COLORREF m_bdColor;
    UINT m_nBgImgID;

    BOOL bPress, bHover;
    Bitmap* m_pbmBG;

    CB_MOUSE _cbMouse;
    void* _pParamMouse;
    BOOL _bCaptured;
public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseHover(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();
    afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};