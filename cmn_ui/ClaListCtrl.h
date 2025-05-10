#pragma once

#include <afxcmn.h>
#include "CListCtrlEx.h"

class ClaListCtrl : public CListCtrlEx
{
	DECLARE_DYNAMIC(ClaListCtrl)

public:
	ClaListCtrl();

	void setHead(const wchar_t* headers);
	int addRecord(const wchar_t* firstItem, ...);
	int addRecordWithIcon(int p_nIconIndex, const wchar_t* firstItem, ...);
	void autoFitWidth();
	int getSelectedIndex();
	void setColor(COLORREF c1, COLORREF c2) {
		_c1 = c1; _c2 = c2;
	};

protected:
	int _nColCnt;
	COLORREF _c1, _c2;
	CFont _font;

protected:
	DECLARE_MESSAGE_MAP()
public:
	void SetFontStyle(const CString& fontFace, int fontSize, BOOL isBold);
//	afx_msg void OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
	//afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};

