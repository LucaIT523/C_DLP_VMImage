// TranlucentButton.cpp : implementation file
//

#include "pch.h"
#include "KCheckbox.h"
#include "Utility.h"
#include "KWnd.h"


// KCheckbox

IMPLEMENT_DYNAMIC(KCheckbox, KWnd)

KCheckbox::KCheckbox()
	: KWnd()
	, m_pCurrImage(NULL)
	, m_bMouseOver(false)
	, m_bPressed(false)
	, m_bTracking(false)
	, m_bSelected(false)
	, m_nPos(0)
	, m_nPosOld(-1)
{
	m_imageList.resize(2, NULL);
}

KCheckbox::~KCheckbox()
{
}


BEGIN_MESSAGE_MAP(KCheckbox, KWnd)
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
END_MESSAGE_MAP()



// KCheckbox message handlers


void KCheckbox::PreSubclassWindow()
{
	//ModifyStyle(0, BS_OWNERDRAW);
	//VERIFY();
	KWnd::PreSubclassWindow();
}

bool KCheckbox::LoadImageList(UINT imageID[2], LPCTSTR lpszResType, HINSTANCE hInstance/* = NULL*/)
{
	DestroyImageList();

	if (imageID == 0)
		return false;

	m_imageList[TWS_NORMAL] = CUtility::LoadImage(imageID[TWS_NORMAL], lpszResType, hInstance);
	if (m_imageList[TWS_NORMAL] == NULL)
		return false;

	m_imageList[1] = CUtility::LoadImage(imageID[1], lpszResType, hInstance);

	RECT rt; GetClientRect(&rt);
	int h = m_imageList[0]->GetHeight();

	VERIFY(SetWindowPos(NULL, -1, -1, rt.right - rt.left, h, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE));
	return true;
}

void KCheckbox::UpdateState()
{
	NotifyRender();
}

void KCheckbox::OnLButtonUp(UINT nFlags, CPoint point)
{
	KWnd::OnLButtonUp(nFlags, point);
	UpdateState();
}

void KCheckbox::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	KWnd::OnKeyUp(nChar, nRepCnt, nFlags);
	UpdateState();
}


void KCheckbox::Render(Gdiplus::Graphics& g)
{
	m_pCurrImage = m_imageList[0];

	int w = m_pCurrImage->GetWidth();
	int h = m_pCurrImage->GetHeight();
	CRect rc;
	GetWindowRect(&rc);
	GetParent()->ScreenToClient(&rc);

	Gdiplus::RectF rect((float)rc.left, (float)rc.top, w, h);

	int nCheck = ((CButton*)this)->GetCheck();
	if (nCheck) {
		g.DrawImage(m_imageList[0], rect);
	}
	else {
		g.DrawImage(m_imageList[1], rect);
	}

	CString szText;
	GetWindowText(szText);
	if (szText.GetLength() > 0)
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

		rc.left += w + 3;

		Gdiplus::RectF rect((float)rc.left, (float)rc.top, (float)rc.Width(), (float)rc.Height());
		g.DrawString(szText, szText.GetLength(), &m_pFont, rect, &format, &brush);
	}

}
