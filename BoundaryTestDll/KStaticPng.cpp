// TranlucentButton.cpp : implementation file
//

#include "pch.h"
#include "KStaticPng.h"
#include "Utility.h"


// KStaticPng

IMPLEMENT_DYNAMIC(KStaticPng, KWnd)

KStaticPng::KStaticPng()
	: KWnd()
	, m_pCurrImage(NULL)
{
	m_imageList.resize(1, NULL);
}

KStaticPng::~KStaticPng()
{
	//DestroyImageList();
}

BEGIN_MESSAGE_MAP(KStaticPng, KWnd)
END_MESSAGE_MAP()

// KStaticPng message handlers

void KStaticPng::PreSubclassWindow()
{
	KWnd::PreSubclassWindow();
}

bool KStaticPng::LoadImageK(UINT imageID, LPCTSTR lpszResType, HINSTANCE hInstance)
{
	DestroyImageList();

	m_imageList[TWS_NORMAL] = CUtility::LoadImage(imageID, lpszResType, hInstance);
	if (m_imageList[TWS_NORMAL] == NULL)
		return false;
	
	m_pCurrImage = m_imageList[TWS_NORMAL];

	VERIFY(SetWindowPos(NULL, -1, -1, m_pCurrImage->GetWidth(), m_pCurrImage->GetHeight(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE));
	return true;
}

void KStaticPng::UpdateState()
{
	Gdiplus::Image* prevImage = m_pCurrImage;

	if (prevImage != m_pCurrImage)
	{
		NotifyRender();
	}
}

void KStaticPng::Render(Gdiplus::Graphics& g)
{
	if (m_pCurrImage != NULL)
	{
		CRect rc;
		GetWindowRect(&rc);
		GetParent()->ScreenToClient(&rc);

		Gdiplus::RectF rect((float)rc.left, (float)rc.top, (float)rc.Width(), (float)rc.Height());
		g.DrawImage(m_pCurrImage, rect);
	}
}
