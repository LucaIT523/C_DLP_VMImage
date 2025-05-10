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
	m_imageList.resize(4, NULL);
	_nColCnt = 0;
	_nSelRow = -1;
	memset(_wszRowFontFace, 0, sizeof(_wszRowFontFace));
	memset(_wszHeadFontFace, 0, sizeof(_wszHeadFontFace));
}

KListCtrl::~KListCtrl()
{
}


BEGIN_MESSAGE_MAP(KListCtrl, KWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
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

void KListCtrl::Render(Gdiplus::Graphics& g)
{
	int nTop, nLeft;
	CRect rc;
	GetWindowRect(&rc);
	GetParent()->ScreenToClient(&rc);

	//	Head
	_rtHeadTotal.left = rc.left;
	_rtHeadTotal.top = rc.top;
	_rtHeadTotal.right = rc.right;
	_rtHeadTotal.bottom = _rtHeadTotal.top + m_imageList[0]->GetHeight();

	nTop = _rtHeadTotal.bottom-1;

	// Body
	_lstRtRows.RemoveAll();
	CListCtrl* pThis = (CListCtrl*)this;
	
	for (int i = 0; i < pThis->GetItemCount(); i++) {
		CRect rtItem;
		rtItem.left = _rtHeadTotal.left;
		rtItem.right = _rtHeadTotal.right;
		rtItem.top = nTop;
		rtItem.bottom = rtItem.top + _rowHeight;
	}
	
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
	g.DrawImage(imgHead, rectRight, head_width*2, 0, head_width, head_height, Gdiplus::UnitPixel);

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

	//
	// Draw body
	//
	int nIndex = 0;
	nTop = _rtHeadTotal.bottom - 1;

	Gdiplus::RectF rectPanel((float)rc.left, (float)nTop, rc.Width(), rc.Height() - _rtHeadTotal.Height() + 1);
	g.DrawImage(m_imageList[2], rectPanel, 0, 0, m_imageList[2]->GetWidth() - 1, m_imageList[2]->GetHeight()-1, Gdiplus::UnitPixel);


	while (nTop < rc.bottom) {
		Gdiplus::RectF rectRowItem((float)rc.left+1, (float)nTop, rc.Width()-2, min(_rowHeight, rc.bottom - nTop));
		Gdiplus::Image* imgRow;

		CRect rt(rectRowItem.X, rectRowItem.Y, rectRowItem.GetRight(), rectRowItem.GetBottom());
		_lstRtRows.Add(rt);

		if (nIndex == _nSelRow) {
			g.DrawImage(m_imageList[3], rectRowItem, 0, 0, m_imageList[3]->GetWidth() - 1, m_imageList[3]->GetHeight(), Gdiplus::UnitPixel);
		}else if (nIndex % 2 != 0) {
			imgRow = m_imageList[1];
			g.DrawImage(imgRow, rectRowItem, 0, 0, imgRow->GetWidth() - 1, imgRow->GetHeight(), Gdiplus::UnitPixel);
		}

		nLeft = 0;
		for (int i = 0; nIndex < pThis->GetItemCount() && i < _nColCnt; i++) {
			CString strText = pThis->GetItemText(nIndex, i);
			int w = pThis->GetColumnWidth(i);
			Gdiplus::RectF rtItem(nLeft + _rtHeadTotal.left, (float)rectRowItem.Y, w, rectRowItem.Height);

			if (!strText.IsEmpty())
				DrawText_(g, strText, rtItem, FALSE);
			nLeft += w;

		}

		nTop += _rowHeight;
		nIndex++;
	}

}

bool KListCtrl::LoadImageList(
	UINT imageIDHead, UINT imageIDRow1, UINT imageIDRow2, UINT imageIDRowSel, 
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

void KListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
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

	UpdateState();

	if (_nMsgID != 0) {
		GetParent()->PostMessageW(_nMsgID, _nSelRow);
	}

	KWnd::OnLButtonDown(nFlags, point);
}


void KListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_UP) {
		if (_nSelRow > 0) {
			_nSelRow--;
			UpdateState();
			GetParent()->PostMessageW(_nMsgID, _nSelRow);
			return;
		}
	}
	else if (nChar == VK_DOWN) {
		if (_nSelRow < GetItemCount() - 1) {
			_nSelRow++;
			UpdateState();
			GetParent()->PostMessageW(_nMsgID, _nSelRow);
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
