#include "pch.h"
#include "ClaListCtrl.h"

#include <sstream>

IMPLEMENT_DYNAMIC(ClaListCtrl, CListCtrlEx)

BEGIN_MESSAGE_MAP(ClaListCtrl, CListCtrlEx)
//ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &ClaListCtrl::OnNMCustomdraw)
END_MESSAGE_MAP()

ClaListCtrl::ClaListCtrl()
 : CListCtrlEx()
{

}

void ClaListCtrl::setHead(const wchar_t* headers)
{
	std::wstring headerString(headers);
	std::wistringstream iss(headerString);
	std::wstring header;
	int nCol = 0;
	while (std::getline(iss, header, L';'))
	{
		InsertColumn(nCol++, header.c_str(), LVCFMT_LEFT, 100); // Assuming a default width of 100
	}
	_nColCnt = nCol;

	
}

int ClaListCtrl::addRecord(const wchar_t* firstItem, ...)
{
    int nRet = -1;

	va_list args;
	va_start(args, firstItem);

	int nItem = GetItemCount();
	nRet = InsertItem(nItem, CString(firstItem));

	int nCol = 1;
	const wchar_t* nextItem = nullptr;
	while (nCol < _nColCnt)
	{
		nextItem = va_arg(args, const wchar_t*);
		SetItemText(nItem, nCol++, CString(nextItem));
	}

	va_end(args);

    return nRet;
}

int ClaListCtrl::addRecordWithIcon(int p_nIconIndex, const wchar_t* firstItem, ...)
{
	int nRet = -1;

	va_list args;
	va_start(args, firstItem);

	int nItem = GetItemCount();
	nRet = InsertItem(LVIF_TEXT|LVIF_IMAGE, nItem, CString(firstItem), 0, 0, p_nIconIndex, 0);

	int nCol = 1;
	const wchar_t* nextItem = nullptr;
	while (nCol < _nColCnt)
	{
		nextItem = va_arg(args, const wchar_t*);
		SetItemText(nItem, nCol++, CString(nextItem));
	}

	va_end(args);

	return nRet;
}

void ClaListCtrl::autoFitWidth()
{
    CHeaderCtrl* pHeader = GetHeaderCtrl();
    int nColumnCount = pHeader->GetItemCount();

    // Temporary variable to store the width needed for the header
    int nHeaderWidth = 0;
    HDITEM hdi = { 0 };
    hdi.mask = HDI_TEXT;
    TCHAR szText[256]; // Buffer to store the header text
    hdi.pszText = szText;
    hdi.cchTextMax = 255;

    // Go through each column
    for (int i = 0; i < nColumnCount; i++)
    {
        // Get the header item
        pHeader->GetItem(i, &hdi);

        // Set the width of the column to the width of the header text
        SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
        nHeaderWidth = GetColumnWidth(i);

        // Autosize the column width to the longest item's text width
        SetColumnWidth(i, LVSCW_AUTOSIZE);
        int nColumnWidth = GetColumnWidth(i);

        // Choose the maximum width between the header and the column content
        SetColumnWidth(i, max(nHeaderWidth, nColumnWidth));
    }
}
int ClaListCtrl::getSelectedIndex()
{
    return GetNextItem(-1, LVNI_SELECTED);

    /*    for (int i = 0; i < GetItemCount(); i++) {
            GetItemState(i, lvn_selec)
        }
        */
        //    return 0;
}
void ClaListCtrl::SetFontStyle(const CString& fontFace, int fontSize, BOOL isBold)
{
    int dpiX = GetDC()->GetDeviceCaps(LOGPIXELSX);
    float scaleX = dpiX / 96.0f;
    fontSize *= scaleX;

    _font.DeleteObject(); // Delete any existing font

    // Create a new font
    _font.CreateFont(
        fontSize,            // Height of the font
        0,                  // Width of the font
        0,                  // Angle of escapement
        0,                  // Base-line angle
        isBold ? FW_BOLD : FW_NORMAL, // Font weight
        0,                  // Italic attribute option
        0,                  // Underline option
        0,                  // Strikeout option
        DEFAULT_CHARSET,   // Character set identifier
        OUT_DEFAULT_PRECIS,// Output precision
        CLIP_DEFAULT_PRECIS,// Clipping precision
        DEFAULT_QUALITY,   // Output quality
        DEFAULT_PITCH | FF_DONTCARE,       // Pitch and family
        fontFace);         // Font face name

    SetFont(&_font, FALSE);
}

/*
void ClaListCtrl::OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

    // Take the default processing unless 

    // we set this to something else below.

    *pResult = CDRF_DODEFAULT;

    // First thing - check the draw stage. If it's the control's prepaint

    // stage, then tell Windows we want messages for every item.


    if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        int rowIndex = pLVCD->nmcd.dwItemSpec;
        if (rowIndex % 2 == 0) {
            pLVCD->clrTextBk = _c1;
        }
        else {
            pLVCD->clrTextBk = _c2;
        }
        *pResult = CDRF_DODEFAULT;
        //*pResult = CDRF_NOTIFYSUBITEMDRAW;
    }
    else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage)
    {
        COLORREF crText, crBkgnd;

        if (0 == pLVCD->iSubItem)
        {
            crText = RGB(255, 0, 0);
            crBkgnd = RGB(128, 128, 255);
        }
        else if (1 == pLVCD->iSubItem)
        {
            crText = RGB(0, 255, 0);
            crBkgnd = RGB(255, 0, 0);
        }
        else
        {
            crText = RGB(128, 128, 255);
            crBkgnd = RGB(0, 0, 0);
        }

        // Store the colors back in the NMLVCUSTOMDRAW struct.

        pLVCD->clrText = crText;
        pLVCD->clrTextBk = crBkgnd;

        // Tell Windows to paint the control itself.

        *pResult = CDRF_DODEFAULT;
    }
}
*/