#include "pch.h"
#include "ClaButtonDraw.h"
#include "ClaResCmn.h"

ClaButtonDraw::ClaButtonDraw()
{
    m_pbmBG = NULL;
    bPress = 0, bHover = 0;
    _cbMouse = NULL;
}

ClaButtonDraw::~ClaButtonDraw()
{
}

BEGIN_MESSAGE_MAP(ClaButtonDraw, CStatic)
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
// ON_WM_MOUSEHOVER()
ON_WM_MOUSEMOVE()
ON_WM_MOUSELEAVE()
//    ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

void ClaButtonDraw::OnPaint()
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

    int r = GetRValue(m_bgColor), g = GetGValue(m_bgColor), b = GetBValue(m_bgColor);

    if (m_pbmBG == NULL) {
        if (!bPress && !bHover)
        {
        }
        else if (bPress)
        {
            r = r * 80 / 100;
            g = g * 80 / 100;
            b = b * 80 / 100;
        }
        else
        {
            r = min(r * 120 / 100, 255);
            g = min(g * 120 / 100, 255);
            b = min(b * 120 / 100, 255);
        }
    }

    // Draw the filled ellipse
    SolidBrush ellipseBrush(Color(r, g, b));
    GetClientRect(&rect); // Get the dimensions of the static control
    Rect rt;
    rt.X = rect.left;
    rt.Y = rect.top;
    rt.Width = rect.right;
    rt.Height = rect.bottom;
    int x = 0, y = 0, radius = 10, width = rt.Width, height = rt.Height;

    GraphicsPath path;
    path.AddArc(x, y, radius * 2, radius * 2, 180, 90);                                          // Top-left corner
    path.AddLine(x + radius, y, x + width - radius, y);                                          // Top side
    path.AddArc(x + width - radius * 2, y, radius * 2, radius * 2, 270, 90);                     // Top-right corner
    path.AddLine(x + width, y + radius, x + width, y + height - radius);                         // Right side
    path.AddArc(x + width - radius * 2, y + height - radius * 2, radius * 2, radius * 2, 0, 90); // Bottom-right corner
    path.AddLine(x + width - radius, y + height, x + radius, y + height);                        // Bottom side
    path.AddArc(x, y + height - radius * 2, radius * 2, radius * 2, 90, 90);                     // Bottom-left corner
    path.CloseFigure();                                                                          // Close the path to create a closed shape

    graphics.FillRectangle(&ellipseBrush, rt); // Fill the ellipse using the control's rectangle
    HBITMAP hBitmap = nullptr;
    //// Draw the ellipse outline
    // Pen ellipsePen(Color(GetRValue(m_bdColor), GetGValue(m_bdColor), GetBValue(m_bdColor)), 2); // Width of 2 pixels
    // graphics.DrawPath(&ellipsePen, &path); // Draw the ellipse outline using the control's rectangle    }
    if (m_pbmBG != NULL)
    {
        int srcX = 0;
        int srcY = 0;
        if (!bPress && !bHover)
        {

        }
        else if (bPress)
        {
            srcX = (m_pbmBG->GetWidth() * 2) / 3;
        }
        else
        {
            srcX = m_pbmBG->GetWidth() / 3;
        }
        Rect destRect(0, 0, rt.Width, rt.Height);
        graphics.DrawImage(m_pbmBG, destRect,srcX, srcY, m_pbmBG->GetWidth() / 3, m_pbmBG->GetHeight(), Gdiplus::UnitPixel);
    }
    else {
        if (!text.IsEmpty())
        {
            if (align == 0)
                dc.DrawText(text, &rect, DT_LEFT | DT_VCENTER );
            else if (align == 1)
                dc.DrawText(text, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
            else
                dc.DrawText(text, &rect, DT_RIGHT);
        }
        else
        {
        }
    }
}

void ClaButtonDraw::setColor(COLORREF bg, COLORREF bd, COLORREF txt)
{
    m_bgColor = bg;
    m_bdColor = bd;
    m_fontColor = txt;
}

void ClaButtonDraw::setBgImage(UINT p_bgImage)
{
    ClaResCmn::GetImage(p_bgImage, &m_pbmBG);
}

void ClaButtonDraw::setFont(const CString &fontFace, int fontSize, BOOL isBold, int p_nAlign)
{
    align = p_nAlign;
    m_font.DeleteObject(); // Delete any existing font

    // Create a new font
    m_font.CreateFont(
        fontSize,                     // Height of the font
        0,                            // Width of the font
        0,                            // Angle of escapement
        0,                            // Base-line angle
        isBold ? FW_BOLD : FW_NORMAL, // Font weight
        0,                            // Italic attribute option
        0,                            // Underline option
        0,                            // Strikeout option
        DEFAULT_CHARSET,              // Character set identifier
        OUT_DEFAULT_PRECIS,           // Output precision
        CLIP_DEFAULT_PRECIS,          // Clipping precision
        DEFAULT_QUALITY,              // Output quality
        DEFAULT_PITCH | FF_DONTCARE,  // Pitch and family
        fontFace);                    // Font face name
}

void ClaButtonDraw::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    if (_cbMouse) {
        SetCapture();
        _bCaptured = TRUE;
        _cbMouse(_pParamMouse, TRUE);
    }

    bPress = TRUE;
    CButton::OnLButtonDown(nFlags, point);
}

void ClaButtonDraw::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    if (_cbMouse) {
        if (_bCaptured) ReleaseCapture();
		_bCaptured = FALSE;
        _cbMouse(_pParamMouse, FALSE);
    }
	bPress = FALSE;
    CButton::OnLButtonUp(nFlags, point);
}

void ClaButtonDraw::OnMouseHover(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    CButton::OnMouseHover(nFlags, point);
}

void ClaButtonDraw::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    bHover = TRUE;

    CButton::OnMouseMove(nFlags, point);
}

void ClaButtonDraw::OnMouseLeave()
{
    // TODO: Add your message handler code here and/or call default
	if (_cbMouse) {
		if (_bCaptured) ReleaseCapture();
		_cbMouse(_pParamMouse, FALSE);
		_bCaptured = FALSE;
	}

    bHover = FALSE;
    CButton::OnMouseLeave();
}

int ClaButtonDraw::OnMouseActivate(CWnd *pDesktopWnd, UINT nHitTest, UINT message)
{
    // TODO: Add your message handler code here and/or call default

    return CButton::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
