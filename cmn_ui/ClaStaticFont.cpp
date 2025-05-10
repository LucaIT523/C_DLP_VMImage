#include "pch.h"
#include "ClaStaticFont.h"

ClaStaticFont::ClaStaticFont()
{
    align = 0;
    valign = 0;
}

ClaStaticFont::~ClaStaticFont()
{
}

BEGIN_MESSAGE_MAP(ClaStaticFont, CStatic)
    ON_MESSAGE(WM_SETTEXT, OnSetText)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_ERASEBKGND()

    ON_WM_PAINT()
END_MESSAGE_MAP()

LRESULT ClaStaticFont::OnSetText(WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = Default();
    Invalidate();
    UpdateWindow();
    return Result;
}

HBRUSH ClaStaticFont::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
    pDC->SetBkMode(TRANSPARENT);
    return (HBRUSH)GetStockObject(NULL_BRUSH);
}

BOOL ClaStaticFont::OnEraseBkgnd(CDC* pDC)
{
    if (m_Bmp.GetSafeHandle() == NULL)
    {
        CRect Rect;
        GetWindowRect(&Rect);
        CWnd* pParent = GetParent();
        ASSERT(pParent);
        pParent->ScreenToClient(&Rect);  //convert our corrdinates to our parents

        //copy what's on the parents at this point
        CDC* pDC = pParent->GetDC();
        CDC MemDC;
        MemDC.CreateCompatibleDC(pDC);
        m_Bmp.CreateCompatibleBitmap(pDC, Rect.Width(), Rect.Height());
        CBitmap* pOldBmp = MemDC.SelectObject(&m_Bmp);
        MemDC.BitBlt(0, 0, Rect.Width(), Rect.Height(), pDC, Rect.left, Rect.top, SRCCOPY);
        MemDC.SelectObject(pOldBmp);
        pParent->ReleaseDC(pDC);
    }
    else //copy what we copied off the parent the first time back onto the parent
    {
        CRect Rect;
        GetClientRect(Rect);
        CDC MemDC;
        MemDC.CreateCompatibleDC(pDC);
        CBitmap* pOldBmp = MemDC.SelectObject(&m_Bmp);
        pDC->BitBlt(0, 0, Rect.Width(), Rect.Height(), &MemDC, 0, 0, SRCCOPY);
        MemDC.SelectObject(pOldBmp);
    }

    return TRUE;
}


void ClaStaticFont::OnPaint()
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
    if (!text.IsEmpty()) {
        int a;
        if (align == 0) {
            a = DT_LEFT;
        }
        else if (align == 1) {
            a = DT_CENTER;
        }
        else {
            a = DT_RIGHT;
        }
        if (valign == 0) {
            a |= DT_TOP | DT_WORDBREAK;
        }
        else if (valign == 1) {
            rect.top += 2;
            a |= DT_VCENTER | DT_SINGLELINE;
        }
        else {
            a |= DT_BOTTOM;
        }

        dc.DrawText(text, &rect, a);
    }
    else {
        Graphics graphics(dc.m_hDC);
        // Draw the filled ellipse
        SolidBrush ellipseBrush(
            Color(
                GetRValue(m_bgColor), 
                GetGValue(m_bgColor),
                GetBValue(m_bgColor))
        );
        CRect rect;
        GetClientRect(&rect); // Get the dimensions of the static control
        Rect rt; rt.X = rect.left; rt.Y = rect.top; rt.Width = rect.right; rt.Height = rect.bottom;
        int x = 0, y = 0, radius = 10, width = rt.Width, height = rt.Height;

        GraphicsPath path;
        path.AddArc(x, y, radius * 2, radius * 2, 180, 90); // Top-left corner
        path.AddLine(x + radius, y, x + width - radius, y); // Top side
        path.AddArc(x + width - radius * 2, y, radius * 2, radius * 2, 270, 90); // Top-right corner
        path.AddLine(x + width, y + radius, x + width, y + height - radius); // Right side
        path.AddArc(x + width - radius * 2, y + height - radius * 2, radius * 2, radius * 2, 0, 90); // Bottom-right corner
        path.AddLine(x + width - radius, y + height, x + radius, y + height); // Bottom side
        path.AddArc(x, y + height - radius * 2, radius * 2, radius * 2, 90, 90); // Bottom-left corner
        path.CloseFigure(); // Close the path to create a closed shape


        graphics.FillPath(&ellipseBrush, &path); // Fill the ellipse using the control's rectangle

        // Draw the ellipse outline
        Pen ellipsePen(Color(GetRValue(m_bdColor), GetGValue(m_bdColor), GetBValue(m_bdColor)), 2); // Width of 2 pixels
        graphics.DrawPath(&ellipsePen, &path); // Draw the ellipse outline using the control's rectangle    }
    }
}

void ClaStaticFont::SetFontColor(COLORREF color)
{
    m_fontColor = color;
}

void ClaStaticFont::SetFontStyle(const CString& fontFace, int fontSize, BOOL isBold, int p_nAlign, int p_nvAlign)
{
    int dpiX = GetDC()->GetDeviceCaps(LOGPIXELSX);
    float scaleX = dpiX / 96.0f;
    fontSize *= scaleX;

    align = p_nAlign;
    valign = p_nvAlign;
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

void ClaStaticFont::SetBackgroundColor(COLORREF color, COLORREF colorBorder)
{
    m_bgColor = color;
    m_bdColor = colorBorder;
    Invalidate(); // Trigger a repaint
}
