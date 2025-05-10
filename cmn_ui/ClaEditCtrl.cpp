#include "pch.h"
#include "ClaEditCtrl.h"

ClaEditCtrl::ClaEditCtrl()
{
}

ClaEditCtrl::~ClaEditCtrl()
{
}

BEGIN_MESSAGE_MAP(ClaEditCtrl, CEdit)
    ON_WM_CTLCOLOR()
//    ON_WM_PAINT()
//    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void ClaEditCtrl::SetFontColor(COLORREF color)
{
    m_fontColor = color;
}

void ClaEditCtrl::SetFontStyle(const CString& fontFace, int fontSize, BOOL isBold, int p_nAlign)
{
    int dpiX = GetDC()->GetDeviceCaps(LOGPIXELSX);
    float scaleX = dpiX / 96.0f;
    fontSize *= scaleX;

    if (p_nAlign == 0) {
        ModifyStyle(ES_LEFT | ES_CENTER | ES_RIGHT, ES_LEFT);
    }
    else if (p_nAlign == 1){
        ModifyStyle(ES_LEFT | ES_CENTER | ES_RIGHT, ES_CENTER);
    }
	else if (p_nAlign == 2) {
		ModifyStyle(ES_LEFT | ES_CENTER | ES_RIGHT, ES_RIGHT);
	}
    align = p_nAlign;
    _font.DeleteObject(); // Delete any existing font

    // Create a new font
    _font.CreateFont(
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

    SetFont(&_font);
}

void ClaEditCtrl::SetBackgroundColor(COLORREF color, COLORREF colorBorder)
{
    m_bgColor = color;
    m_bdColor = colorBorder;

	_Brush.DeleteObject();
	_Brush.CreateSolidBrush(m_bgColor);
    Invalidate(); // Trigger a repaint
}

HBRUSH ClaEditCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetTextColor(m_fontColor);

    return _Brush;
}


void ClaEditCtrl::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: Add your message handler code here
    // Do not call CEdit::OnPaint() for painting messages

	//CRect rt; GetClientRect(rt);

 //   dc.FillRect(rt, &_Brush);
	return ;
}


BOOL ClaEditCtrl::OnEraseBkgnd(CDC* pDC)
{

    return CEdit::OnEraseBkgnd(pDC);
}
