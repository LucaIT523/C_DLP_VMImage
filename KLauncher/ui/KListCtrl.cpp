// TranlucentButton.cpp : implementation file
//

#include "pch.h"
#include "KListCtrl.h"
#include "Utility.h"
#include "KWnd.h"
#include <sstream>

#define LD_LIST_PADDING 24
// KListCtrl

IMPLEMENT_DYNAMIC(KListCtrl, KWnd)

KListCtrl::KListCtrl()
	: KWnd()
{
	_nMsgID = 0;
	m_imageList.resize(5, NULL);
	_nColCnt = 0;
	_nSelRow = -1;
	memset(_wszRowFontFace, 0, sizeof(_wszRowFontFace));
	memset(_wszHeadFontFace, 0, sizeof(_wszHeadFontFace));
	m_nDelta = 0;
	m_nMaxDelta = 0;
	_rtPanel.X = 0; _rtPanel.Y = 0; _rtPanel.Width = 0; _rtPanel.Height = 0;
	_bScrolling = FALSE;
}

KListCtrl::~KListCtrl()
{
}


BEGIN_MESSAGE_MAP(KListCtrl, KWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// KListCtrl message handlers


void KListCtrl::PreSubclassWindow()
{
	//ModifyStyle(0, BS_OWNERDRAW);
	//VERIFY();
	KWnd::PreSubclassWindow();
}

void KListCtrl::UpdateState()
{
	NotifyRender();
}

void KListCtrl::DrawText_(Gdiplus::Graphics& g, const wchar_t* text, Gdiplus::RectF& rt, BOOL p_bHead)
{
	Gdiplus::StringFormat format;
	format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	wchar_t* wzFont;

	Gdiplus::Font fontHead(_wszHeadFontFace, _headFontSize, _isHeadFontBold ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular);
	Gdiplus::Font fontRow(_wszRowFontFace, _rowFontSize, _isRowFontBold ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular);
	Gdiplus::SolidBrush brushHead(Gdiplus::Color(GetRValue(_headTextColor), GetGValue(_headTextColor), GetBValue(_headTextColor)));
	Gdiplus::SolidBrush brushRow(Gdiplus::Color(GetRValue(_rowTextColor), GetGValue(_rowTextColor), GetBValue(_rowTextColor)));

	rt.X += LD_LIST_PADDING;

	if (p_bHead)
		g.DrawString(text, wcslen(text), &fontHead, rt, &format, &brushHead);
	else
		g.DrawString(text, wcslen(text), &fontRow, rt, &format, &brushRow);
}

void KListCtrl::_drawHeader(Gdiplus::Graphics& g)
{
	int nTop, nLeft;
	CRect rc;
	GetWindowRect(&rc);
	GetParent()->ScreenToClient(&rc);
	CListCtrl* pThis = (CListCtrl*)this;

	_rtHeadTotal.left = rc.left;
	_rtHeadTotal.top = rc.top;
	_rtHeadTotal.right = rc.right;
	_rtHeadTotal.bottom = _rtHeadTotal.top + m_imageList[0]->GetHeight();

	//
	// Draw header
	//
	Gdiplus::Image* imgHead = m_imageList[0];
	int head_width = imgHead->GetWidth() / 3;
	int head_height = imgHead->GetHeight();

	Gdiplus::RectF rectLeft((float)_rtHeadTotal.left, (float)_rtHeadTotal.top, head_width, head_height);
	Gdiplus::RectF rectRight((float)_rtHeadTotal.right - head_width, (float)_rtHeadTotal.top, head_width, head_height);
	Gdiplus::RectF rectMiddle(rectLeft.GetRight(), (float)_rtHeadTotal.top, _rtHeadTotal.Width() - head_width * 2, head_height);

	g.DrawImage(imgHead, rectLeft, 0, 0, head_width, head_height, Gdiplus::UnitPixel);
	g.DrawImage(imgHead, rectMiddle, head_width, 0, head_width, head_height, Gdiplus::UnitPixel);
	g.DrawImage(imgHead, rectRight, head_width * 2, 0, head_width, head_height, Gdiplus::UnitPixel);

	nLeft = 0;
	for (int i = 0; i < _nColCnt; i++) {
		LVCOLUMN col; memset(&col, 0, sizeof(col));
		col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
		TCHAR szColumnText[100];
		col.pszText = szColumnText;
		col.cchTextMax = sizeof(szColumnText) / sizeof(TCHAR);

		pThis->GetColumn(i, &col);
		int w = pThis->GetColumnWidth(i);
		Gdiplus::RectF rtHeadItem(nLeft + _rtHeadTotal.left, (float)_rtHeadTotal.top, w, head_height);
		if (wcslen(col.pszText) > 0)
			DrawText_(g, col.pszText, rtHeadItem, TRUE);

		nLeft += w;
	}
}

void KListCtrl::_drawBody(Gdiplus::Graphics& g)
{
	int nTop, nLeft;
	CRect rc;
	GetWindowRect(&rc);
	GetParent()->ScreenToClient(&rc);
	CListCtrl* pThis = (CListCtrl*)this;

	_rtHeadTotal.left = rc.left;
	_rtHeadTotal.top = rc.top;
	_rtHeadTotal.right = rc.right;
	_rtHeadTotal.bottom = _rtHeadTotal.top + m_imageList[0]->GetHeight();

	int height = rc.Height() - _rtHeadTotal.Height();
	if (height < pThis->GetItemCount() * _rowHeight) {
		m_nMaxDelta = pThis->GetItemCount() * _rowHeight - height;
	}
	else {
		m_nMaxDelta = 0;
	}
	_lstRtRows.RemoveAll();
	m_nDelta = min(max(m_nDelta, 0), m_nMaxDelta);

	// draw background
	nTop = _rtHeadTotal.bottom;
	Gdiplus::RectF rectPanel((float)rc.left, (float)nTop, rc.Width(), rc.Height() - _rtHeadTotal.Height() + 1);
	_rtPanel = rectPanel;
	g.SetClip(rectPanel);

	g.DrawImage(m_imageList[2], rectPanel, 0, 0, m_imageList[2]->GetWidth() - 1, m_imageList[2]->GetHeight() - 1, Gdiplus::UnitPixel);


	for (int i = 0; i < pThis->GetItemCount(); i++) {
		int left, top, right, bottom;
		left = rc.left + 1, top = nTop - m_nDelta + i * _rowHeight, right = rc.right - 1, bottom = top + _rowHeight;
		if (top > rectPanel.GetBottom() || bottom < rectPanel.GetTop()) {
			CRect rt(0, 0, 0, 0);
			_lstRtRows.Add(rt);
			continue;
		}
		else {
			int topBg = max(top, rectPanel.GetTop());
			int bottomBg = min(bottom, rectPanel.GetBottom());
			CRect rt(left, topBg, right, bottomBg);
			_lstRtRows.Add(rt);
		}
		
		Gdiplus::RectF rtItem((float)left, (float)top, right - left, bottom - top);

		//.	draw background of item
		if (i == _nSelRow) {
			g.DrawImage(m_imageList[3], rtItem, 0, 0, m_imageList[3]->GetWidth() - 1, m_imageList[3]->GetHeight(), Gdiplus::UnitPixel);
		}
		else if (i % 2 != 0) {
			g.DrawImage(m_imageList[1], rtItem, 0, 0, m_imageList[1]->GetWidth() - 1, m_imageList[1]->GetHeight(), Gdiplus::UnitPixel);
		}

		nLeft = 0;
		//. draw text of item
		for (int j = 0; j < _nColCnt; j++) {
			CString strText = pThis->GetItemText(i, j);
			int w = pThis->GetColumnWidth(j);
			Gdiplus::RectF rtItem(nLeft + _rtHeadTotal.left, (float)rtItem.Y, w, rtItem.Height);

			if (!strText.IsEmpty())
				DrawText_(g, strText, rtItem, FALSE);
			nLeft += w;
		}
	}

	g.ResetClip();
}

void KListCtrl::_drawScrollbar(Gdiplus::Graphics& g)
{
	if (m_imageList[4] == NULL) return;
	if (m_nMaxDelta == 0) return;
	int range = _rtPanel.Height - m_imageList[4]->GetHeight();
	int top = (m_nDelta * range) / m_nMaxDelta + _rtPanel.Y;
	int bottom = top + m_imageList[4]->GetHeight();
	int left = _rtPanel.GetRight() - m_imageList[4]->GetWidth() - 2;
	int right = left + m_imageList[4]->GetWidth();

	Gdiplus::RectF rtScroll(left, top, right - left, bottom - top);
	_rtScroll = rtScroll;
	g.DrawImage(m_imageList[4], rtScroll, 0, 0, m_imageList[4]->GetWidth(), m_imageList[4]->GetHeight(), Gdiplus::UnitPixel);
}

void KListCtrl::Render(Gdiplus::Graphics& g)
{
	_drawHeader(g);
	_drawBody(g);
	_drawScrollbar(g);
}

bool KListCtrl::LoadImageList(
	UINT imageIDHead, UINT imageIDRow1, UINT imageIDRow2, UINT imageIDRowSel, UINT imageIDScroll,
	LPCTSTR lpszResType /*= _T("PNG")*/, HINSTANCE hInstance /*= NULL*/)
{
	ASSERT(imageIDHead > 0);
	ASSERT(imageIDRow1 > 0);

	m_imageList[0] = CUtility::LoadImage(imageIDHead, lpszResType, hInstance);
	m_imageList[1] = CUtility::LoadImage(imageIDRow1, lpszResType, hInstance);

	if (imageIDRow2) {
		m_imageList[2] = CUtility::LoadImage(imageIDRow2, lpszResType, hInstance);
	}

	if (imageIDRowSel) {
		m_imageList[3] = CUtility::LoadImage(imageIDRowSel, lpszResType, hInstance);
	}

	if (imageIDScroll) {
		m_imageList[4] = CUtility::LoadImage(imageIDScroll, lpszResType, hInstance);
	}

	return 0;
}

void KListCtrl::setHead(const wchar_t* headers)
{
	CListCtrl* pThis = (CListCtrl*)this;
	std::wstring headerString(headers);
	std::wistringstream iss(headerString);
	std::wstring header;
	int nCol = 0;
	while (std::getline(iss, header, L';'))
	{
		pThis->InsertColumn(nCol++, header.c_str(), LVCFMT_LEFT, 100); // Assuming a default width of 100
	}
	_nColCnt = nCol;

	UpdateState();
}

void KListCtrl::setHeadWidth(int firstItem, ...)
{
	int nRet = -1;
	CListCtrl* pThis = (CListCtrl*)this;

	va_list args;
	va_start(args, firstItem);

	int nItem = pThis->GetItemCount();
	_headWidths[0] = firstItem;
	pThis->SetColumnWidth(0, firstItem);

	int nCol = 1;
	int nextItem = 0;
	while (nCol < _nColCnt)
	{
		nextItem = va_arg(args, int);
		_headWidths[nCol] = nextItem;
		pThis->SetColumnWidth(nCol, nextItem);
		nCol++;
	}

	va_end(args);

	UpdateState();
}

int KListCtrl::addRecord(const wchar_t* firstItem, ...)
{
	int nRet = -1;
	CListCtrl* pThis = (CListCtrl*)this;

	va_list args;
	va_start(args, firstItem);

	int nItem = pThis->GetItemCount();
	nRet = pThis->InsertItem(nItem, CString(firstItem));

	int nCol = 1;
	const wchar_t* nextItem = nullptr;
	while (nCol < _nColCnt)
	{
		nextItem = va_arg(args, const wchar_t*);
		pThis->SetItemText(nItem, nCol++, CString(nextItem));
	}

	va_end(args);

	UpdateState();

	return nRet;
}

void KListCtrl::_OnSelChange(int p_nIndex)
{
	if (_nSelRow != -1) {
		int nTop = _nSelRow * _rowHeight - m_nDelta;
		int nBottom = nTop + _rowHeight;
		if (nTop < 0) {
			m_nDelta += nTop;
		}
		if (nBottom > _rtPanel.Height) {
			m_nDelta += (nBottom - _rtPanel.Height);
		}
	}

	if (_nMsgID != 0) {
		GetParent()->PostMessageW(_nMsgID, _nSelRow);
	}
	UpdateState();
}

void KListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rtScroll(_rtScroll.X, _rtScroll.Y, _rtScroll.GetRight(), _rtScroll.GetBottom());
	GetParent()->ClientToScreen(rtScroll);
	ScreenToClient(rtScroll);
	if (PtInRect(rtScroll, point)) {
		_bScrolling = true;
		_ptStart = point;
		m_nDeltaOld = m_nDelta;
		SetCapture();
		return;
	}
	
	_nSelRow = -1;
	for (int i = 0; i < _lstRtRows.GetCount(); i++) {
		CRect rt = _lstRtRows[i];
		GetParent()->ClientToScreen(rt);
		ScreenToClient(rt);

		if (PtInRect(rt, point)) {
			_nSelRow = i;
			break;
		}
	}
	if (_nSelRow >= ((CListCtrl*)this)->GetItemCount()) {
		_nSelRow = -1;
	}

	_OnSelChange(_nSelRow);

	KWnd::OnLButtonDown(nFlags, point);
}


void KListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_UP) {
		if (_nSelRow > 0) {
			_nSelRow--;
			_OnSelChange(_nSelRow);
			return;
		}
	}
	else if (nChar == VK_DOWN) {
		if (_nSelRow < GetItemCount() - 1) {
			_nSelRow++;
			_OnSelChange(_nSelRow);
			return;
		}
	}
	KWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

int KListCtrl::GetItemCount()
{
	CListCtrl* pThis = (CListCtrl*)this;
	return pThis->GetItemCount();
}

int KListCtrl::DeleteItem(int p_nItem)
{
	_nSelRow = -1;
	CListCtrl* pThis = (CListCtrl*)this;
	return pThis->DeleteItem(p_nItem);
}

int KListCtrl::SetItemData(int p_nItem, DWORD_PTR p_dwData)
{
	CListCtrl* pThis = (CListCtrl*)this;
	return pThis->SetItemData(p_nItem, p_dwData);
}

DWORD_PTR KListCtrl::GetItemData(int p_nItem)
{
	CListCtrl* pThis = (CListCtrl*)this;
	return pThis->GetItemData(p_nItem);
}

LRESULT KListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return KWnd::WindowProc(message, wParam, lParam);
}

BOOL KListCtrl::PreTranslateMessage(MSG* pMsg)
{
	return KWnd::PreTranslateMessage(pMsg);
}


BOOL KListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	BOOL bRet = KWnd::OnMouseWheel(nFlags, zDelta, pt);
	m_nDelta -= zDelta;
	m_nDelta = max(m_nDelta, 0);
	m_nDelta = min(m_nDelta, m_nMaxDelta);
	UpdateState();
	return bRet;
}


void KListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (_bScrolling)
	{
		ReleaseCapture();
		_bScrolling = false;
	}
	KWnd::OnLButtonUp(nFlags, point);
}


void KListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (_bScrolling) {
		int delta = point.y - _ptStart.y;
		int total = _rtPanel.Height - m_imageList[4]->GetHeight();
		m_nDelta = m_nDeltaOld + m_nMaxDelta * delta / total;
		m_nDelta = min(max(m_nDelta, 0), m_nMaxDelta);
		UpdateState();
	}

	KWnd::OnMouseMove(nFlags, point);
}
