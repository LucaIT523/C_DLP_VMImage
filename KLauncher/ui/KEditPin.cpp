// TranlucentButton.cpp : implementation file
//

#include "pch.h"
#include "KEditPin.h"
#include "Utility.h"


// KEditPin

IMPLEMENT_DYNAMIC(KEditPin, KWnd)

KEditPin::KEditPin()
	: KWnd()
{
	_bFocus = FALSE;
	m_imageList.resize(4, NULL);
}

KEditPin::~KEditPin()
{
	//DestroyImageList();
}

BEGIN_MESSAGE_MAP(KEditPin, KWnd)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

// KEditPin message handlers

void KEditPin::PreSubclassWindow()
{
	KWnd::PreSubclassWindow();
}

void KEditPin::UpdateState()
{
	NotifyRender();
}

void KEditPin::Render(Gdiplus::Graphics& g)
{
	CString szText;
	GetWindowText(szText);
	if (1)
	{
		if (szText.IsEmpty()) {
			if (_bFocus == FALSE) {
				m_pCurrImage = m_imageList[0];
			}
			else {
				m_pCurrImage = m_imageList[1];
			}
		}
		else {
			if (_bFocus == FALSE) {
				m_pCurrImage = m_imageList[2];
			}
			else {
				m_pCurrImage = m_imageList[3];
			}
		}
		{
			CRect rc;
			GetWindowRect(&rc);
			GetParent()->ScreenToClient(&rc);

			Gdiplus::RectF rect((float)rc.left, (float)rc.top, (float)rc.Width(), (float)rc.Height());
			g.DrawImage(m_pCurrImage, rect);
		}
	}
}

bool KEditPin::LoadImageK(UINT imageID[4], LPCTSTR lpszResType, HINSTANCE hInstance)
{
	DestroyImageList();

	m_imageList[0] = CUtility::LoadImage(imageID[0], lpszResType, hInstance);
	m_imageList[1] = CUtility::LoadImage(imageID[1], lpszResType, hInstance);
	m_imageList[2] = CUtility::LoadImage(imageID[2], lpszResType, hInstance);
	m_imageList[3] = CUtility::LoadImage(imageID[3], lpszResType, hInstance);

	m_pCurrImage = m_imageList[0];

	VERIFY(SetWindowPos(NULL, -1, -1, m_pCurrImage->GetWidth(), m_pCurrImage->GetHeight(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE));
	return true;
}

LRESULT KEditPin::OnSetText(WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = Default();
	UpdateState();
	return Result;
}


void KEditPin::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	KWnd::OnChar(nChar, nRepCnt, nFlags);

	// TODO: Add your message handler code here and/or call default
	UpdateState();
}


void KEditPin::OnSetFocus(CWnd* pOldWnd)
{
	KWnd::OnSetFocus(pOldWnd);
	_bFocus = TRUE;
	UpdateState();
}


void KEditPin::OnKillFocus(CWnd* pNewWnd)
{
	KWnd::OnKillFocus(pNewWnd);
	_bFocus = FALSE;
	UpdateState();
}
