// TranlucentButton.cpp : implementation file
//

#include "pch.h"
#include "KRadioPng.h"
#include "Utility.h"
#include "KWnd.h"


// KRadioPng

IMPLEMENT_DYNAMIC(KRadioPng, KWnd)

KRadioPng::KRadioPng()
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
}

KRadioPng::~KRadioPng()
{
}


BEGIN_MESSAGE_MAP(KRadioPng, KWnd)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// KRadioPng message handlers


void KRadioPng::PreSubclassWindow()
{
	//ModifyStyle(0, BS_OWNERDRAW);
	//VERIFY();
	KWnd::PreSubclassWindow();
}

bool KRadioPng::LoadImageList(UINT imageID[TWS_BUTTON_NUM], LPCTSTR lpszResType, HINSTANCE hInstance/* = NULL*/)
{
	DestroyImageList();

	if (imageID == 0)
		return false;

	m_imageList[0] = CUtility::LoadImage(imageID[0], lpszResType, hInstance);
	if (m_imageList[0] == NULL)
		return false;

	if (imageID[1] != 0)
		m_imageList[1] = CUtility::LoadImage(imageID[1], lpszResType, hInstance);

	if (imageID[2] != 0)
		m_imageList[2] = CUtility::LoadImage(imageID[2], lpszResType, hInstance);

	if (imageID[3] != 0)
		m_imageList[3] = CUtility::LoadImage(imageID[3], lpszResType, hInstance);


	m_pCurrImage = m_imageList[0];
	
	int w = m_pCurrImage->GetWidth(); int h = m_pCurrImage->GetHeight();

	VERIFY(SetWindowPos(NULL, -1, -1, m_pCurrImage->GetWidth(), m_pCurrImage->GetHeight(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE));

	m_bUpdated = TRUE;
	return true;
}

void KRadioPng::UpdateState(int num)
{
	Gdiplus::Image* prevImage = m_pCurrImage;
	if (m_imageList[num] != NULL) {
		m_pCurrImage = m_imageList[num];
	}

	if (prevImage != m_pCurrImage || m_bUpdated)
	{
		NotifyRender();
		m_bUpdated = FALSE;
	}
}

void KRadioPng::OnMouseMove(UINT nFlags, CPoint point)
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

LRESULT KRadioPng::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bMouseOver = false;
	m_bTracking = false;
	return 0;
}

LRESULT KRadioPng::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	m_bMouseOver = true;
	return 0;
}

void KRadioPng::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bPressed = true;
	KWnd::OnLButtonDown(nFlags, point);
}

void KRadioPng::OnLButtonUp(UINT nFlags, CPoint point)
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

	KWnd::OnLButtonUp(nFlags, point);
}

void KRadioPng::SetChecked(bool b)
{
	m_bSelected = b;
}

bool KRadioPng::IsChecked() const
{ 
	return m_bSelected;
}

void KRadioPng::Render(Gdiplus::Graphics& g)
{
	if (m_pCurrImage != NULL)
	{
		int w = m_pCurrImage->GetWidth();
		int h = m_pCurrImage->GetHeight();
		CRect rc;
		GetWindowRect(&rc);
		GetParent()->ScreenToClient(&rc);

		Gdiplus::RectF rect((float)rc.left, (float)rc.top, (float)rc.Width(), (float)rc.Height());
		g.DrawImage(m_pCurrImage, rect);
//		g.DrawImage(m_pCurrImage, (INT)rect.X, (INT)rect.Y, (INT)m_nPos * w, (INT)0, (INT)w, (INT)h, Gdiplus::UnitDisplay/*UnitPixel*/);
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
