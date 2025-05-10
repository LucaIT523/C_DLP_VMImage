#include "pch.h"
#include "ClaDialogPng.h"

IMPLEMENT_DYNAMIC(ClaDialogPng, CDialog)

BEGIN_MESSAGE_MAP(ClaDialogPng, CDialog)
	ON_WM_PAINT()
    ON_WM_NCHITTEST()
END_MESSAGE_MAP()

void ClaDialogPng::OnPaint()
{
    CRect rt;
    GetWindowRect(rt);

    CPaintDC dc(this); // device context for painting
    Graphics graphics(dc.m_hDC);

    SolidBrush brushBG(Color(
        GetRValue(colorBG),
        GetGValue(colorBG),
        GetBValue(colorBG)));

	SolidBrush brushTitle(Color(
		GetRValue(colorTITLE),
		GetGValue(colorTITLE),
		GetBValue(colorTITLE)));

	Pen penBorder(Color(
		GetRValue(colorBD),
		GetGValue(colorBD),
		GetBValue(colorBD)));


    // Draw the background image
    if (m_pBitmap)
    {
        graphics.DrawImage(m_pBitmap, 0, 0, rt.Width(), rt.Height());
    }
    else {
        Rect rtp; rtp.X = 0; rtp.Y = 0; rtp.Width = rt.Width(); rtp.Height = rt.Height();
        graphics.FillRectangle(&brushBG, rtp);

        int dpiX = dc.GetDeviceCaps(LOGPIXELSX);
        float scaleX = dpiX / 96.0f;

        rtp.Height = _nTitleHeight+2* scaleX+1;
        graphics.FillRectangle(&brushTitle, rtp);

		rtp.Height = rt.Height()-1;
        rtp.Width--;
		graphics.DrawRectangle(&penBorder, rtp);
    }
}


LRESULT ClaDialogPng::OnNcHitTest(CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    return HTCAPTION;

    return CDialog::OnNcHitTest(point);
}
