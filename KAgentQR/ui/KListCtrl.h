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
	virtual void Render(Gdiplus::Graphics& g);

public:
	bool LoadImageList(
		UINT imageIDHead, UINT imageIDRow1, UINT imageIDRow2, UINT imageIDRowSel,
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
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


