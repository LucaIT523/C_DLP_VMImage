// TranlucentButton.cpp : implementation file
//

#include "pch.h"
#include "KButtonPng.h"
#include "Utility.h"
#include "KWnd.h"


// KButtonPng

IMPLEMENT_DYNAMIC(KButtonPng, KWnd)

KButtonPng::KButtonPng()
	: KWnd()
	, m_pCurrImage(NULL)
	, m_bMouseOver(false)
	, m_bPressed(false)
	, m_bTracking(false)
	, m_bSelected(false)
	, m_nPos(0)
	, m_nPosOld(-1)
	, m_bUpdated(FALSE)
{
	memset(_wszFont, 0, sizeof(_wszFont));
	m_imageList.resize(TWS_BUTTON_NUM, NULL);
	m_bResize = TRUE;
}

KButtonPng::~KButtonPng()
{
}


BEGIN_MESSAGE_MAP(KButtonPng, KWnd)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// KButtonPng message handlers


void KButtonPng::PreSubclassWindow()
{
	//ModifyStyle(0, BS_OWNERDRAW);
	//VERIFY();
	KWnd::PreSubclassWindow();
}

bool KButtonPng::LoadImageList(UINT imageID[TWS_BUTTON_NUM], LPCTSTR lpszResType, HINSTANCE hInstance/* = NULL*/, BOOL p_bResize /*= TRUE*/)
{
	DestroyImageList();

	if (imageID == 0)
		return false;

	m_imageList[TWS_NORMAL] = CUtility::LoadImage(imageID[TWS_NORMAL], lpszResType, hInstance);
	if (m_imageList[TWS_NORMAL] == NULL)
		return false;

	if (imageID[TWS_MOUSEOVER] != 0)
		m_imageList[TWS_MOUSEOVER] = CUtility::LoadImage(imageID[TWS_MOUSEOVER], lpszResType, hInstance);

	if (imageID[TWS_PRESSED] != 0)
		m_imageList[TWS_PRESSED] = CUtility::LoadImage(imageID[TWS_PRESSED], lpszResType, hInstance);

	if (imageID[TWS_DISABLED] != 0)
		m_imageList[TWS_DISABLED] = CUtility::LoadImage(imageID[TWS_DISABLED], lpszResType, hInstance);


	m_pCurrImage = m_imageList[TWS_NORMAL];
	
	int w = m_pCurrImage->GetWidth(); int h = m_pCurrImage->GetHeight();

	m_bResize = p_bResize;
	if (p_bResize)
		VERIFY(SetWindowPos(NULL, -1, -1, m_pCurrImage->GetWidth(), m_pCurrImage->GetHeight(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE));

	m_bUpdated = TRUE;
	UpdateState();
	return true;
}

void KButtonPng::UpdateState()
{
	Gdiplus::Image* prevImage = m_pCurrImage;

	if (m_bMouseOver)
	{
		if (m_bPressed)
		{
			if (m_imageList[TWS_PRESSED] != NULL)
				m_pCurrImage = m_imageList[TWS_PRESSED];
			else
				m_pCurrImage = m_imageList[TWS_NORMAL];
		}
		else
		{
			if (m_imageList[TWS_MOUSEOVER] != NULL)
				m_pCurrImage = m_imageList[TWS_MOUSEOVER];
			else
				m_pCurrImage = m_imageList[TWS_NORMAL];
		}
	}
	else if (m_bDisabled)
	{
		if (m_imageList[TWS_DISABLED] != NULL)
			m_pCurrImage = m_imageList[TWS_DISABLED];
		else
			m_pCurrImage = m_imageList[TWS_NORMAL];
	}
	else if (m_bSelected)
	{
		if (m_imageList[TWS_SELECTED] != NULL)
			m_pCurrImage = m_imageList[TWS_SELECTED];
		else
			m_pCurrImage = m_imageList[TWS_NORMAL];
	}
	else
	{
		m_pCurrImage = m_imageList[TWS_NORMAL];
	}

	if (prevImage != m_pCurrImage || m_bUpdated)
	{
		NotifyRender();
		m_bUpdated = FALSE;
	}
}

void KButtonPng::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = (_TrackMouseEvent(&tme) == FALSE) ? false : true;
	}

	KWnd::OnMouseMove(nFlags, point);
}

LRESULT KButtonPng::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bMouseOver = false;
	m_bTracking = false;
	UpdateState();
	return 0;
}

LRESULT KButtonPng::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	m_bMouseOver = true;
	UpdateState();
	return 0;
}

void KButtonPng::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bPressed = true;
	UpdateState();
	KWnd::OnLButtonDown(nFlags, point);
}

void KButtonPng::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bPressed = false;
	RECT rc;
	GetClientRect(&rc);
	BOOL bIn = ::PtInRect(&rc, point);
	if (bIn)
	{
		UINT uStyle = (UINT)::GetWindowLong(m_hWnd, GWL_STYLE) & 0xff;
		if ((uStyle == BS_RADIOBUTTON) || (uStyle == BS_CHECKBOX) || (uStyle == BS_AUTOCHECKBOX) || (uStyle == BS_AUTORADIOBUTTON))
			m_bSelected = !m_bSelected;
		else
			m_bSelected = false;
	}
	else
	{
		m_bMouseOver = false;
	}

	UpdateState();
	KWnd::OnLButtonUp(nFlags, point);
}

void KButtonPng::SetChecked(bool b)
{
	m_bSelected = b;
	UpdateState();
}

bool KButtonPng::IsChecked() const
{ 
	return m_bSelected;
}

void KButtonPng::Render(Gdiplus::Graphics& g)
{
	if (m_pCurrImage != NULL)
	{
		int w = m_pCurrImage->GetWidth();
		int h = m_pCurrImage->GetHeight();
		CRect rc;
		GetWindowRect(&rc);
		GetParent()->ScreenToClient(&rc);

		Gdiplus::RectF rect((float)rc.left, (float)rc.top, (float)rc.Width(), (float)rc.Height());
		if (m_bResize)
			g.DrawImage(m_pCurrImage, rect);
		else
			g.DrawImage(m_pCurrImage, rect, 0, 0, (INT)w, (INT)h, Gdiplus::UnitPixel);
	}

	CString szText;
	GetWindowText(szText);
	if (szText.GetLength() > 0 && wcslen(_wszFont) > 0)
	{
		Gdiplus::FontStyle style;
		if (_bold) {
			style = Gdiplus::FontStyleBold;
		}
		else {
			style = Gdiplus::FontStyleRegular;
		}
		Gdiplus::Font m_pFont(_wszFont, _w, style);

		Gdiplus::StringFormat format;

		format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		if (_align == 0) {
			format.SetAlignment(Gdiplus::StringAlignmentNear);
		}
		else if (_align == 1) {
			format.SetAlignment(Gdiplus::StringAlignmentCenter);
		}
		else {
			format.SetAlignment(Gdiplus::StringAlignmentFar);
		}

		Gdiplus::SolidBrush brush(Gdiplus::Color(GetRValue(_c), GetGValue(_c), GetBValue(_c)));

		CRect rc;
		GetWindowRect(&rc);
		GetParent()->ScreenToClient(&rc);

		Gdiplus::RectF rect((float)rc.left, (float)rc.top, (float)rc.Width(), (float)rc.Height());
		g.DrawString(szText, szText.GetLength(), &m_pFont, rect, &format, &brush);
	}

}
