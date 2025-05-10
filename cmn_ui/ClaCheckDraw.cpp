#include "pch.h"
#include "ClaCheckDraw.h"
#include "ClaResCmn.h"

ClaCheckDraw::ClaCheckDraw()
{
    bPress = 0, bHover = 0;
    m_pCheck = NULL, m_pUncheck = NULL;
}

ClaCheckDraw::~ClaCheckDraw()
{
}

BEGIN_MESSAGE_MAP(ClaCheckDraw, CStatic)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEHOVER()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSELEAVE()
    ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


void ClaCheckDraw::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    CRect rect;
    GetClientRect(&rect);

    dc.SetBkMode(TRANSPARENT);

    // Set the text color
    dc.SetTextColor(m_fontColor);
    dc.SelectObject(m_font);

    // Draw the text
    CString text;
    GetWindowText(text);

    Graphics graphics(dc.m_hDC);

    int bCheck = GetCheck();

    Rect rtCheck;
    rtCheck.X = 0; rtCheck.Y = (rect.Height() - 16) / 2; rtCheck.Width = 16; rtCheck.Height = 16;

    SolidBrush brushEraser(Color(GetRValue(m_bgColor), GetGValue(m_bgColor), GetBValue(m_bgColor)));

    Rect rtBack; rtBack.X = 0; rtBack.Y = 0; rtBack.Width = rect.Width(); rtBack.Height = rect.Height();

    graphics.FillRectangle(&brushEraser, rtBack);

    if (bCheck) {
        graphics.DrawImage(m_pCheck, rtCheck);
    }
    else {
        graphics.DrawImage(m_pUncheck, rtCheck);
    }

    if (!text.IsEmpty()) {
        rect.left = 20;
        rect.bottom--;
        if (align == 0)
            dc.DrawText(text, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        else if (align == 1)
            dc.DrawText(text, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
        else
            dc.DrawText(text, &rect, DT_RIGHT);
    }
    else {
    }
}

void ClaCheckDraw::setColor(COLORREF bg, COLORREF bd, COLORREF txt)
{
    m_bgColor = bg; m_bdColor = bd; m_fontColor = txt;
}
void ClaCheckDraw::setFont(const CString& fontFace, int fontSize, BOOL isBold, int p_nAlign)
{
    align = p_nAlign;
    m_font.DeleteObject(); // Delete any existing font

    // Create a new font
    m_font.CreateFont(
        fontSize,            // Height of the font
        0,                  // Width of the font
        0,                  // Angle of escapement
        0,                  // Base-line angle
        isBold ? FW_BOLD : FW_NORMAL, // Font weight
        0,                  // Italic attribute option
        0,                  // Underline option
        0,                  // Strikeout option
        DEFAULT_CHARSET,   // Character set identifier
        OUT_DEFAULT_PRECIS,// Output precision
        CLIP_DEFAULT_PRECIS,// Clipping precision
        DEFAULT_QUALITY,   // Output quality
        DEFAULT_PITCH | FF_DONTCARE,       // Pitch and family
        fontFace);         // Font face name
}

void ClaCheckDraw::setImage(UINT p_nCheck, UINT p_nUncheck)
{
    ClaResCmn::GetImage(p_nCheck, &m_pCheck);
    ClaResCmn::GetImage(p_nUncheck, &m_pUncheck);
}


void ClaCheckDraw::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    bPress = TRUE;
    CButton::OnLButtonDown(nFlags, point);
}


void ClaCheckDraw::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    bPress = FALSE;
    CButton::OnLButtonUp(nFlags, point);
    Invalidate(TRUE);
}


void ClaCheckDraw::OnMouseHover(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    CButton::OnMouseHover(nFlags, point);
}


void ClaCheckDraw::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    bHover = TRUE;

    CButton::OnMouseMove(nFlags, point);
}


void ClaCheckDraw::OnMouseLeave()
{
    // TODO: Add your message handler code here and/or call default
    bHover = FALSE;
    CButton::OnMouseLeave();
}


int ClaCheckDraw::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
    // TODO: Add your message handler code here and/or call default

    return CButton::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
