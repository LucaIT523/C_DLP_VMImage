// TranlucentButton.cpp : implementation file
//

#include "pch.h"
#include "KStaticMenu.h"
#include "Utility.h"

#define LD_KMENU_HEIGHT_TOP		30
#define LD_KMENU_HEIGHT_MID		25
#define LD_KMENU_HEIGHT_BTM		30

#define LD_KMENU_TILE			10

// KStaticMenu

IMPLEMENT_DYNAMIC(KStaticMenu, KWnd)

KStaticMenu::KStaticMenu()
	: KWnd()
	, m_pCurrImage(NULL)
{
	m_imageList.resize(2, NULL);
}

KStaticMenu::~KStaticMenu()
{
	//DestroyImageList();
}

BEGIN_MESSAGE_MAP(KStaticMenu, KWnd)
END_MESSAGE_MAP()

// KStaticMenu message handlers

void KStaticMenu::PreSubclassWindow()
{
	KWnd::PreSubclassWindow();
}

bool KStaticMenu::LoadImageK(UINT imageID, UINT imageIDSel, LPCTSTR lpszResType, HINSTANCE hInstance)
{
	DestroyImageList();

	m_imageList[0] = CUtility::LoadImage(imageID, lpszResType, hInstance);
	m_imageList[1] = CUtility::LoadImage(imageIDSel, lpszResType, hInstance);

	return true;
}

void KStaticMenu::UpdateState()
{
	Gdiplus::Image* prevImage = m_pCurrImage;

	if (prevImage != m_pCurrImage)
	{
		NotifyRender();
	}
}

void KStaticMenu::_DrawBG(Gdiplus::Graphics& g, Gdiplus::Image* pImage)
{
	// Draw Background of Item
	CRect rc;
	GetWindowRect(&rc);
	GetParent()->ScreenToClient(&rc);

	Gdiplus::RectF rcLeftTop	(rc.left				 , rc.top				, LD_KMENU_TILE					, LD_KMENU_TILE);
	Gdiplus::RectF rcMiddleTop	(rc.left + LD_KMENU_TILE , rc.top				, rc.Width() - LD_KMENU_TILE*2	, LD_KMENU_TILE);
	Gdiplus::RectF rcRightTop	(rc.right - LD_KMENU_TILE, rc.top				, LD_KMENU_TILE					, LD_KMENU_TILE);

	Gdiplus::RectF rcLeftMiddle	(rc.left				 , rc.top + LD_KMENU_TILE, LD_KMENU_TILE				, rc.Height() - LD_KMENU_TILE * 2);
	Gdiplus::RectF rcMiddleMiddle(rc.left + LD_KMENU_TILE, rc.top + LD_KMENU_TILE, rc.Width() - LD_KMENU_TILE*2	, rc.Height() - LD_KMENU_TILE * 2);
	Gdiplus::RectF rcRightMiddle(rc.right - LD_KMENU_TILE, rc.top + LD_KMENU_TILE, LD_KMENU_TILE				, rc.Height() - LD_KMENU_TILE * 2);

	Gdiplus::RectF rcLeftBottom	(rc.left				 , rc.bottom - LD_KMENU_TILE, LD_KMENU_TILE					, LD_KMENU_TILE);
	Gdiplus::RectF rcMiddleBottom(rc.left + LD_KMENU_TILE, rc.bottom - LD_KMENU_TILE, rc.Width() - LD_KMENU_TILE*2	, LD_KMENU_TILE);
	Gdiplus::RectF rcRightBottom(rc.right - LD_KMENU_TILE, rc.bottom - LD_KMENU_TILE, LD_KMENU_TILE					, LD_KMENU_TILE);

	g.DrawImage(pImage, rcLeftTop	, 0									, 0, LD_KMENU_TILE, LD_KMENU_TILE, Gdiplus::UnitPixel);
	g.DrawImage(pImage, rcMiddleTop	, LD_KMENU_TILE						, 0, LD_KMENU_TILE, LD_KMENU_TILE, Gdiplus::UnitPixel);
	g.DrawImage(pImage, rcRightTop	, pImage->GetWidth() - LD_KMENU_TILE, 0, LD_KMENU_TILE, LD_KMENU_TILE, Gdiplus::UnitPixel);

	g.DrawImage(pImage, rcLeftMiddle	, 0									, LD_KMENU_TILE, LD_KMENU_TILE, LD_KMENU_TILE, Gdiplus::UnitPixel);
	g.DrawImage(pImage, rcMiddleMiddle	, LD_KMENU_TILE						, LD_KMENU_TILE, LD_KMENU_TILE, LD_KMENU_TILE, Gdiplus::UnitPixel);
	g.DrawImage(pImage, rcRightMiddle	, pImage->GetWidth() - LD_KMENU_TILE, LD_KMENU_TILE, LD_KMENU_TILE, LD_KMENU_TILE, Gdiplus::UnitPixel);

	g.DrawImage(pImage, rcLeftBottom	, 0									, pImage->GetHeight() - LD_KMENU_TILE, LD_KMENU_TILE, LD_KMENU_TILE, Gdiplus::UnitPixel);
	g.DrawImage(pImage, rcMiddleBottom	, LD_KMENU_TILE						, pImage->GetHeight() - LD_KMENU_TILE, LD_KMENU_TILE, LD_KMENU_TILE, Gdiplus::UnitPixel);
	g.DrawImage(pImage, rcRightBottom	, pImage->GetWidth() - LD_KMENU_TILE, pImage->GetHeight() - LD_KMENU_TILE, LD_KMENU_TILE, LD_KMENU_TILE, Gdiplus::UnitPixel);
}

int KStaticMenu::_getHeight(int p_nIndex)
{
	if (p_nIndex == 0) return LD_KMENU_HEIGHT_TOP;
	if (p_nIndex == m_lstItems.GetCount() - 1) return LD_KMENU_HEIGHT_BTM;
	return LD_KMENU_HEIGHT_MID;
}

void KStaticMenu::_DrawBody(Gdiplus::Graphics& g, int p_nIndex)
{
	//.	Get Rectangle of this item
	int top = 0;
	for (int i = 0; i < p_nIndex; i++) 
		top += _getHeight(i);
	int bottom = top + _getHeight(p_nIndex);

	CRect rc;
	GetWindowRect(&rc);
	GetParent()->ScreenToClient(&rc);
}

void KStaticMenu::Render(Gdiplus::Graphics& g)
{
	CRect rtWindow;
	GetClientRect(rtWindow);

	//.	calc total size
	int nBodyCount = m_lstItems.GetCount() - 2;
	int nHeight = LD_KMENU_HEIGHT_TOP + LD_KMENU_HEIGHT_BTM + nBodyCount * LD_KMENU_HEIGHT_MID;

	//.	resize control
	SetWindowPos(NULL, -1, -1, rtWindow.Width(), nHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);

	_DrawBG(g, m_imageList[1]);
}

void KStaticMenu::addItem(BOOL p_bHeader, const wchar_t* p_wszText, UINT iconID, UINT p_nMsgID) {
	KMenuItem item;
	item.isHeader = p_bHeader;
	wcscpy_s(item.wszText, 64, p_wszText);
	if (iconID)
		item.m_pIcon = CUtility::LoadImage(iconID, L"PNG", NULL);
	else
		item.m_pIcon = NULL;
	item.nMsgID = p_nMsgID;

	m_lstItems.Add(item);
}