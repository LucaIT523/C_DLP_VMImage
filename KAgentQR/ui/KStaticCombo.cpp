// TranlucentButton.cpp : implementation file
//

#include "pch.h"
#include "KStaticCombo.h"
#include "Utility.h"
#include <sstream>

// KStaticCombo
#define LD_SPACE_MAIN_LIST	(3)

IMPLEMENT_DYNAMIC(KStaticCombo, KWnd)

KStaticCombo::KStaticCombo()
	: KWnd()
{
	m_isActive = false;
	m_nHover = -1;
	m_nCurSel = -1;
	m_paddingLeft = 10;
	m_paddingRight = 10;
	m_imageList.resize(TWS_COMBO_NUM, NULL);
	_msgID = 0;
}

KStaticCombo::~KStaticCombo()
{
	//DestroyImageList();
}

BEGIN_MESSAGE_MAP(KStaticCombo, KWnd)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

// KStaticCombo message handlers

void KStaticCombo::PreSubclassWindow()
{
	KWnd::PreSubclassWindow();
}

void KStaticCombo::UpdateState()
{
	NotifyRender();
}

LRESULT KStaticCombo::OnSetText(WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = Default();
	UpdateState();
	return Result;
}


void KStaticCombo::Render(Gdiplus::Graphics& g)
{
	CRect rc;
	GetWindowRect(&rc);
	GetParent()->ScreenToClient(&rc);

	Gdiplus::Image* pMainImage;

	/////////////////////////////////////////////////////////////////////
	//
	// Main Box
	//

	//.	Main Box background
	if (m_isActive) {
		pMainImage = m_imageList[TWS_COMBO_ACTIVE];
	}
	else {
		pMainImage = m_imageList[TWS_COMBO_NORMAL];
	}
	float left = (float)rc.left;
	float top = (float)rc.top;
	Gdiplus::RectF rect((float)rc.left, (float)rc.top, (float)pMainImage->GetWidth(), (float)pMainImage->GetHeight());
	g.DrawImage(pMainImage, rect);

	//.	Main Box Text
	Gdiplus::RectF rectTitle((float)rect.GetLeft() + 10, top + 5, (float)rect.Width - 10, (float)rect.Height - 10);
	if (m_nCurSel >= 0) {
		DrawText_(g, _items[m_nCurSel], rectTitle, FALSE);
	}

	////////////////////////////////////////////////////////////////////////
	//
	// Combo is not opened
	//
	if (!m_isActive) {
		return;
	}

	///////////////////////////////////////////////////////////////////////
	//
	//	Draw List
	//

	//.	calc top
	top += (float)pMainImage->GetHeight() + LD_SPACE_MAIN_LIST;
	
	//.	not support items < 2
	if (_items.GetCount() < 2) return;

	Gdiplus::Image* pItemImage;

	_rtItems.RemoveAll();

	//.	draw background;
	for (int i = 0; i < _items.GetCount(); i++) {
		if (i == 0) {
			if (i == m_nHover) {
				pItemImage = m_imageList[TWS_COMBO_FIRST_HOVER];
			}
			else {
				pItemImage = m_imageList[TWS_COMBO_FIRST_NORMAL];
			}
		}
		else if (i == _items.GetCount() - 1) {
			if (i == m_nHover) {
				pItemImage = m_imageList[TWS_COMBO_LAST_HOVER];
			}
			else {
				pItemImage = m_imageList[TWS_COMBO_LAST_NORMAL];
			}
		}
		else {
			if (i == m_nHover) {
				pItemImage = m_imageList[TWS_COMBO_MIDDLE_HOVER];
			}
			else {
				pItemImage = m_imageList[TWS_COMBO_MIDDLE_NORMAL];
			}
		}

		//	Draw item background;
		rect.Y = top;
		rect.Height = pItemImage->GetHeight();
		g.DrawImage(pItemImage, rect);

		RECT rtItem;
		rtItem.left = rect.X, rtItem.right = rtItem.left + rect.Width, rtItem.top = rect.Y, rtItem.bottom = rtItem.top + rect.Height;
		GetParent()->ClientToScreen(&rtItem);
		ScreenToClient(&rtItem);
		_rtItems.Add(rtItem);

		rectTitle.Y = top;
		rectTitle.Height = pItemImage->GetHeight();
		DrawText_(g, _items[i], rectTitle, FALSE);

		top += pItemImage->GetHeight();
	}
}

void KStaticCombo::addItem(const wchar_t* p_wszItem) {
	RECT rtMain;
	GetClientRect(&rtMain);

	_items.Add(p_wszItem);
}

void KStaticCombo::DrawText_(Gdiplus::Graphics& g, const wchar_t* text, Gdiplus::RectF& rt, BOOL p_bHead)
{
	Gdiplus::StringFormat format;
	format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	wchar_t* wzFont;

	Gdiplus::Font fontRow(_wszFont, _w, _bold ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular);
	Gdiplus::SolidBrush brushRow(Gdiplus::Color(GetRValue(_c), GetGValue(_c), GetBValue(_c)));

	g.DrawString(text, wcslen(text), &fontRow, rt, &format, &brushRow);
}

void KStaticCombo::_setWindowPos()
{
	int windowWidth = m_imageList[TWS_COMBO_NORMAL]->GetWidth();
	int windowHeight = m_imageList[TWS_COMBO_NORMAL]->GetHeight();
	if (m_isActive) {
		switch (_items.GetCount())
		{
		case 0:
		case 1:
			break;
		default:
			windowWidth = m_imageList[TWS_COMBO_FIRST_NORMAL]->GetWidth();
			windowHeight += m_imageList[TWS_COMBO_FIRST_NORMAL]->GetHeight() * 2;
			windowHeight += m_imageList[TWS_COMBO_MIDDLE_NORMAL]->GetHeight() * (_items.GetCount() - 2);
			break;
		}
	}

	RECT rt;
	GetWindowRect(&rt);
	GetParent()->ScreenToClient(&rt);
	MoveWindow(rt.left, rt.top, windowWidth, windowHeight);
	//VERIFY(SetWindowPos(NULL, -1, -1, windowWidth, windowHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE));
	return;
}

bool KStaticCombo::LoadImageList(UINT imageID[TWS_COMBO_NUM], LPCTSTR lpszResType, HINSTANCE hInstance/* = NULL*/, BOOL p_bResize /*= TRUE*/)
{
	DestroyImageList();

	if (imageID == 0)
		return false;

	m_imageList[TWS_COMBO_NORMAL] = CUtility::LoadImage(imageID[TWS_COMBO_NORMAL], lpszResType, hInstance);
	if (m_imageList[TWS_COMBO_NORMAL] == NULL)
		return false;

	if (imageID[TWS_COMBO_ACTIVE] != 0)
		m_imageList[TWS_COMBO_ACTIVE] = CUtility::LoadImage(imageID[TWS_COMBO_ACTIVE], lpszResType, hInstance);

	if (imageID[TWS_COMBO_FIRST_NORMAL] != 0)
		m_imageList[TWS_COMBO_FIRST_NORMAL] = CUtility::LoadImage(imageID[TWS_COMBO_FIRST_NORMAL], lpszResType, hInstance);

	if (imageID[TWS_COMBO_FIRST_HOVER] != 0)
		m_imageList[TWS_COMBO_FIRST_HOVER] = CUtility::LoadImage(imageID[TWS_COMBO_FIRST_HOVER], lpszResType, hInstance);

	if (imageID[TWS_COMBO_LAST_NORMAL] != 0)
		m_imageList[TWS_COMBO_LAST_NORMAL] = CUtility::LoadImage(imageID[TWS_COMBO_LAST_NORMAL], lpszResType, hInstance);

	if (imageID[TWS_COMBO_LAST_HOVER] != 0)
		m_imageList[TWS_COMBO_LAST_HOVER] = CUtility::LoadImage(imageID[TWS_COMBO_LAST_HOVER], lpszResType, hInstance);

	if (imageID[TWS_COMBO_MIDDLE_NORMAL] != 0)
		m_imageList[TWS_COMBO_MIDDLE_NORMAL] = CUtility::LoadImage(imageID[TWS_COMBO_MIDDLE_NORMAL], lpszResType, hInstance);

	if (imageID[TWS_COMBO_MIDDLE_HOVER] != 0)
		m_imageList[TWS_COMBO_MIDDLE_HOVER] = CUtility::LoadImage(imageID[TWS_COMBO_MIDDLE_HOVER], lpszResType, hInstance);

		//VERIFY(SetWindowPos(NULL, -1, -1, m_pCurrImage->GetWidth(), m_pCurrImage->GetHeight(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE));
	UpdateState();
	return true;
}

void KStaticCombo::setCurSel(int p_nItem) {
	if (p_nItem >= _items.GetCount() || p_nItem < 0) {
		m_nCurSel = -1;
	}
	else {
		m_nCurSel = p_nItem;
	}
	m_isActive = FALSE;
	_setWindowPos();
	UpdateState();
}

void KStaticCombo::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_isActive) {
		for (int i = 0; i < _rtItems.GetCount(); i++) {
			if (PtInRect(&_rtItems[i], point)) {
				m_nCurSel = i;
				break;
			}
		}
	}else{

	}
	m_isActive = !m_isActive;
	_setWindowPos();
	UpdateState();

	KWnd::OnLButtonDown(nFlags, point);

	if (!m_isActive) {
		GetParent()->PostMessageW(_msgID);
	}
}

void KStaticCombo::OnMouseMove(UINT nFlags, CPoint point)
{
	m_nHover = -1;

	for (int i = 0; i < _rtItems.GetCount(); i ++ ) {
		if (PtInRect(&_rtItems[i], point)) {
			m_nHover = i;
			break;
		}
	}
	UpdateState();
	KWnd::OnMouseMove(nFlags, point);
}


void KStaticCombo::OnKillFocus(CWnd* pNewWnd)
{
	m_isActive = false;
	KWnd::OnKillFocus(pNewWnd);

	_setWindowPos();
	UpdateState();
}
