#pragma once

#include "KWnd.h"

// KListCtrl
class KListCtrl : public KWnd
{
	DECLARE_DYNAMIC(KListCtrl)

public:
	KListCtrl();
	virtual ~KListCtrl();
	
	void DrawText_(Gdiplus::Graphics& g, const wchar_t* text, Gdiplus::RectF& rt, BOOL p_bHead);
	void _drawHeader(Gdiplus::Graphics& g);
	void _drawBody(Gdiplus::Graphics& g);
	void _drawScrollbar(Gdiplus::Graphics& g);
	virtual void Render(Gdiplus::Graphics& g);

public:
	bool LoadImageList(
		UINT imageIDHead, UINT imageIDRow1, UINT imageIDRow2, UINT imageIDRowSel, UINT imageIDScroll,
		LPCTSTR lpszResType = _T("PNG"), HINSTANCE hInstance = NULL);

	void setHeadConfig(const wchar_t* wszFontFace, int fontSize, BOOL isBold, COLORREF textColor) {
		wcscpy_s(_wszHeadFontFace, 64, wszFontFace);
		_headFontSize = fontSize;
		_isHeadFontBold = isBold;
		_headTextColor = textColor;
	};

	void setRowConfig(const wchar_t* wszFontFace, int fontSize, BOOL isBold, COLORREF textColor, int h) {
		wcscpy_s(_wszRowFontFace, 64, wszFontFace);
		_rowFontSize = fontSize;
		_isRowFontBold = isBold;
		_rowTextColor = textColor;
		_rowHeight = h;
	};

	void setHead(const wchar_t* headers);
	void setHeadWidth(int firstItem, ...);
	int addRecord(const wchar_t* firstItem, ...);

	int GetItemCount();
	int DeleteItem(int p_nItem);
	int SetItemData(int p_nItem, DWORD_PTR p_dwData);
	DWORD_PTR GetItemData(int p_nItem);

	void setMessageID(UINT p_nMsgID) {
		_nMsgID = p_nMsgID;
	}
	int getCurSel() { return _nSelRow; };

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	virtual void UpdateState();


protected:
	CRect _rtHeadTotal;
	CArray<CRect, CRect&> _lstRtRows;
	int _headWidths[30];

	int _nColCnt;

	wchar_t _wszRowFontFace[64];
	int _rowFontSize;
	BOOL _isRowFontBold;
	COLORREF _rowTextColor;
	int _rowHeight;

	wchar_t _wszHeadFontFace[64];
	int _headFontSize;
	BOOL _isHeadFontBold;
	COLORREF _headTextColor;
	int _nSelRow;

	UINT _nMsgID;
	Gdiplus::RectF _rtPanel;
	Gdiplus::RectF _rtScroll;
	bool _bScrolling;
public:
	void _OnSelChange(int p_nIndex);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	int m_nDelta;
	int m_nDeltaOld;
	int m_nMaxDelta;
	CPoint _ptStart;
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


