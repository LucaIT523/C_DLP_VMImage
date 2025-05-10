// TranlucentButton.cpp : implementation file
//

#include "pch.h"
#include "KProgress.h"
#include "Utility.h"
#include "KWnd.h"


// KProgress

IMPLEMENT_DYNAMIC(KProgress, KWnd)

KProgress::KProgress()
	: KWnd()
{
	m_imageList.resize(2, NULL);
	m_pBackground = NULL;
	m_pFilled = NULL;
}

KProgress::~KProgress()
{
}


BEGIN_MESSAGE_MAP(KProgress, KWnd)
END_MESSAGE_MAP()



// KProgress message handlers


void KProgress::PreSubclassWindow()
{
	//ModifyStyle(0, BS_OWNERDRAW);
	//VERIFY();
	KWnd::PreSubclassWindow();
}

void KProgress::UpdateState()
{
	NotifyRender();
}

void KProgress::Render(Gdiplus::Graphics& g)
{
	CRect rc;
	GetWindowRect(&rc);
	GetParent()->ScreenToClient(&rc);
	int w, h;
	w = rc.Width();
	h = rc.Height();

	Gdiplus::RectF rect((float)rc.left, (float)rc.top, w, h);
	if (m_pBackground) {
		g.DrawImage(m_pBackground, rect);
	}
	else {
		Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 0));
	}
	int nMin, nMax;
	((CProgressCtrl*)this)->GetRange(nMin, nMax);

	int nPos = ((CProgressCtrl*)this)->GetPos();
	w = w * (nPos - nMin) / (nMax - nMin);

	Gdiplus::RectF rectNow((float)rc.left, (float)rc.top, w, h);
	if (m_pBackground) {
		//g.DrawImage(m_pBackground, rect);
		g.DrawImage(m_pFilled, rectNow);
	}
	else {
		Gdiplus::SolidBrush brushNow(Gdiplus::Color(0, 255, 0));
		//Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 0));
		g.FillRectangle(&brushNow, rectNow);
	}
}

void KProgress::setPosition(int p_nPosition)
{
	((CProgressCtrl*)this)->SetPos(p_nPosition);
	UpdateState();
}

void KProgress::setRange(int p_nMin, int p_nMax)
{
	((CProgressCtrl*)this)->SetRange(p_nMin, p_nMax);
	UpdateState();
}
bool KProgress::SetImageBackground(UINT imageID, LPCTSTR lpszResType /*= _T("PNG")*/, HINSTANCE hInstance /*= NULL*/)
{
	m_pBackground = CUtility::LoadImage(imageID, lpszResType, hInstance);
	return true;
}
bool KProgress::SetImageFilled(UINT imageID, LPCTSTR lpszResType /*= _T("PNG")*/, HINSTANCE hInstance /*= NULL*/)
{
	m_pFilled = CUtility::LoadImage(imageID, lpszResType, hInstance);
	return true;
}