// CListCtrlEx.cpp : 
//

#include "pch.h"
#include "CListCtrlEx.h"

// CListCtrlEx

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)

CListCtrlEx::CListCtrlEx()
: m_nRowHeight(0)
, m_fontHeight(12)
, m_fontWith(0)
{
	m_color = RGB(0,0,0);
}

CListCtrlEx::~CListCtrlEx()
{
}

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	ON_WM_MEASUREITEM()
	ON_WM_MEASUREITEM_REFLECT()
END_MESSAGE_MAP()

void CListCtrlEx::PreSubclassWindow()
{
	ModifyStyle(0,LVS_OWNERDRAWFIXED);
	CListCtrl::PreSubclassWindow();
	CHeaderCtrl *pHeader = GetHeaderCtrl();
	m_Header.SubclassWindow(pHeader->GetSafeHwnd());
}

void CListCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	TCHAR lpBuffer[256];

	LV_ITEM lvi;

	lvi.mask = LVIF_TEXT | LVIF_PARAM ;
	lvi.iItem = lpDrawItemStruct->itemID ; 
	lvi.iSubItem = 0;
	lvi.pszText = lpBuffer ;
	lvi.cchTextMax = sizeof(lpBuffer);
	VERIFY(GetItem(&lvi));

	LV_COLUMN lvc, lvcprev ;
	::ZeroMemory(&lvc, sizeof(lvc));
	::ZeroMemory(&lvcprev, sizeof(lvcprev));
	lvc.mask = LVCF_WIDTH | LVCF_FMT;
	lvcprev.mask = LVCF_WIDTH | LVCF_FMT;

	CDC* pDC;
	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rtClient;
	GetClientRect(&rtClient);
	for ( int nCol=0; GetColumn(nCol, &lvc); nCol++)
	{
		if ( nCol > 0 ) 
		{
			// Get Previous Column Width in order to move the next display item
			GetColumn(nCol-1, &lvcprev) ;
			lpDrawItemStruct->rcItem.left += lvcprev.cx ;
			lpDrawItemStruct->rcItem.right += lpDrawItemStruct->rcItem.left; 
		}

		CRect rcItem;   
		if (!GetSubItemRect(lpDrawItemStruct->itemID,nCol,LVIR_LABEL,rcItem))   
			continue;   

		::ZeroMemory(&lvi, sizeof(lvi));
		lvi.iItem = lpDrawItemStruct->itemID;
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iSubItem = nCol;
		lvi.pszText = lpBuffer;
		lvi.cchTextMax = sizeof(lpBuffer);
		VERIFY(GetItem(&lvi));
		CRect rcTemp;
		rcTemp = rcItem;

		if (nCol==0)
		{
			rcTemp.left -=2;
		}

		if ( lpDrawItemStruct->itemState & ODS_SELECTED )
		{
			//pDC->FillSolidRect(&rcTemp, GetSysColor(COLOR_HIGHLIGHT)) ;
			pDC->FillSolidRect(&rcTemp, RGB(62, 70, 132)) ;
			//pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			pDC->SetTextColor(RGB(255,255,255)) ;
		}
		else
		{
			if (lvi.iItem % 2 == 0) {
				pDC->FillSolidRect(rcTemp,_c1);
			}
			else {
				pDC->FillSolidRect(rcTemp,_c2);
			}
			//COLORREF color = GetBkColor();
			//pDC->FillSolidRect(rcTemp,color);

			//stColor *bgColor=FindColBackColor(nCol);
			//if (bgColor!=NULL)
			//{
			//	pDC->FillSolidRect(rcTemp,bgColor->rgb);
			//}

			//bgColor=FindItemBackColor(nCol,lpDrawItemStruct->itemID);
			//if (bgColor!=NULL)
			//{
			//	pDC->FillSolidRect(rcTemp,bgColor->rgb);
			//}
			
			pDC->SetTextColor(m_color);
		}

		pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));

		UINT   uFormat    = DT_CENTER ;
		if (m_Header.m_Format[nCol]=='0')
		{
			uFormat = DT_LEFT;
		}
		else if (m_Header.m_Format[nCol]=='1')
		{
			uFormat = DT_CENTER;
		}
		else if (m_Header.m_Format[nCol]=='2')
		{
			uFormat = DT_RIGHT;
		}
		TEXTMETRIC metric;
		pDC->GetTextMetrics(&metric);
		int ofst;
		ofst = rcItem.Height() - metric.tmHeight;
		rcItem.OffsetRect(0,ofst/2);
		//pDC->SetTextColor(m_color);

		//stColor *textColor=FindColTextColor(nCol);
		//if (textColor!=NULL)
		//{
		//	pDC->SetTextColor(textColor->rgb);
		//}

		//textColor=FindItemTextColor(nCol,lpDrawItemStruct->itemID);
		//if (textColor!=NULL)
		//{
		//	pDC->SetTextColor(textColor->rgb);
		//}
		CFont nFont ,* nOldFont; 
		nFont.CreateFont(m_fontHeight,0,0,0,0,FALSE,FALSE,0,0,0,0,0,0,_TEXT("Arial"));//
		nOldFont = pDC->SelectObject(&nFont);

		rcItem.left += 5;
		DrawText(lpDrawItemStruct->hDC, lpBuffer, wcslen(lpBuffer), 
			&rcItem, uFormat) ;

		pDC->SelectStockObject(SYSTEM_FONT) ;
	}

}

void CListCtrlEx::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}
void CListCtrlEx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (m_nRowHeight>0)
	{
		lpMeasureItemStruct->itemHeight = m_nRowHeight;
	}
}
int CListCtrlEx::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat /* = LVCFMT_LEFT */, int nWidth /* = -1 */, int nSubItem /* = -1 */)
{
	m_Header.m_HChar.Add(lpszColumnHeading);
	if (nFormat==LVCFMT_LEFT)
	{
		m_Header.m_Format = m_Header.m_Format + L"0";
	}
	else if (nFormat==LVCFMT_CENTER)
	{
		m_Header.m_Format = m_Header.m_Format + L"1";
	}
	else if (nFormat==LVCFMT_RIGHT)
	{
		m_Header.m_Format = m_Header.m_Format + L"2";
	}
	else
	{
		m_Header.m_Format = m_Header.m_Format + L"1";
	}
	return CListCtrl::InsertColumn(nCol,lpszColumnHeading,nFormat,nWidth,nSubItem);
}
// Gradient - 
void CListCtrlEx::SetHeaderBackColor(int R, int G, int B, int Gradient) //
{
	m_Header.m_R = R;
	m_Header.m_G = G;
	m_Header.m_B = B;
	m_Header.m_Gradient = Gradient;
}
void CListCtrlEx::SetHeaderBackColorC(COLORREF c, int Gradient)
{
	m_Header.m_B = c >> 16;
	m_Header.m_G = (c >> 8) & 0xFF;
	m_Header.m_R = c & 0xFF;
	m_Header.m_Gradient = Gradient;
}

// 
void CListCtrlEx::SetHeaderHeight(float Height) //
{
	m_Header.m_Height = Height;
}

stColor* CListCtrlEx::FindColBackColor(int col) //
{
	for (POSITION pos = m_ptrListCol.GetHeadPosition();pos!=NULL;)
	{
		stColor *pColor = (stColor*)m_ptrListCol.GetNext(pos);
		if (pColor->nCol==col)
		{
			return pColor;
		}
	}

	return NULL;
}

void CListCtrlEx::SetColBackColor(int col,COLORREF color) //
{
	stColor *pColor  = FindColBackColor(col);

	if(pColor==NULL)
	{
		pColor=new stColor;
		pColor->nCol = col;
		pColor->rgb = color;
		m_ptrListCol.AddTail(pColor);
	}
	else
	{
		pColor->rgb = color;
	}
}

stColor* CListCtrlEx::FindItemBackColor(int col,int row) //
{
	for (POSITION pos = m_ptrListItem.GetHeadPosition();pos!=NULL;)
	{
		stColor *pColor = (stColor*)m_ptrListItem.GetNext(pos);
		if (pColor->nCol==col&&pColor->nRow==row)
		{
			return pColor;
		}
	}
	return NULL;
}

void CListCtrlEx::SetItemBackColor(int col,int row,COLORREF color) //
{
	stColor *pColor=FindItemBackColor(col, row);
	
	if(pColor==NULL)
	{
		pColor= new stColor;
		pColor->nCol = col;
		pColor->nRow = row;
		pColor->rgb = color;
		m_ptrListItem.AddTail(pColor);
	}
	else
	{
		pColor->rgb = color;
	}
}
void CListCtrlEx::SetRowHeigt(int nHeight) //
{
	m_nRowHeight = nHeight;

	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
}
void CListCtrlEx::SetHeaderFontHW(int nHeight,int nWith) //
{
	m_Header.m_fontHeight = nHeight;
	int dpiX = GetDC()->GetDeviceCaps(LOGPIXELSX);
	float scaleX = dpiX / 96.0f;
	m_Header.m_fontHeight *= scaleX;

	m_Header.m_fontWith = nWith;
}
void CListCtrlEx::SetHeaderTextColor(COLORREF color) //
{
	m_Header.m_color = color;
}
BOOL CListCtrlEx::SetTextColor(COLORREF cr)  //
{
	m_color = cr;
	return TRUE;
}
void CListCtrlEx::SetFontSize(int nHeight,int nWith) //
{
	m_fontHeight = nHeight;
	int dpiX = GetDC()->GetDeviceCaps(LOGPIXELSX);
	float scaleX = dpiX / 96.0f;
	m_fontHeight *= scaleX;
	m_fontWith = nWith;
}

void CListCtrlEx::SetBgColor(COLORREF c1, COLORREF c2)
{
	_c1 = c1; _c2 = c2;
}

stColor* CListCtrlEx::FindColTextColor(int col)
{
	for (POSITION pos = m_colTextColor.GetHeadPosition();pos!=NULL;)
	{
		stColor *pColor = (stColor*)m_colTextColor.GetNext(pos);
		if (pColor->nCol==col)
		{
			return pColor;
		}
	}
	return NULL;
}

void CListCtrlEx::SetColTextColor(int col,COLORREF color)
{
	stColor *pColor = FindColTextColor(col);

	if(pColor==NULL)
	{
		pColor=new stColor;
		pColor->nCol = col;
		pColor->rgb = color;
		m_colTextColor.AddTail(pColor);
	}
	else
	{
		pColor->rgb = color;
	}
}


stColor* CListCtrlEx::FindItemTextColor(int col,int row)
{
	for (POSITION pos = m_ItemTextColor.GetHeadPosition();pos!=NULL;)
	{
		stColor *pColor = (stColor*)m_ItemTextColor.GetNext(pos);
		if (pColor->nCol==col&&pColor->nRow==row)
		{
			return pColor;
		}
	}
	return NULL;
}

void CListCtrlEx::SetItemTextColor(int col,int row,COLORREF color)
{
	stColor *pColor = FindItemTextColor(col, row);

	if(pColor==NULL)
	{
		pColor=new stColor;
		pColor->nCol = col;
		pColor->nRow = row;
		pColor->rgb = color;
		m_ItemTextColor.AddTail(pColor);
	}
	else
	{
		pColor->rgb = color;
	}
}