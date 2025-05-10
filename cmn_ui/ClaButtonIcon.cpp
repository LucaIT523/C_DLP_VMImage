#include "pch.h"
#include "ClaButtonIcon.h"

IMPLEMENT_DYNAMIC(ClaButtonIcon, CButton)

ClaButtonIcon::ClaButtonIcon()
    : m_hIcon(nullptr)
{
}

ClaButtonIcon::~ClaButtonIcon()
{
    if (m_hIcon != nullptr)
        DestroyIcon(m_hIcon);
}

void ClaButtonIcon::SetIcon(HICON hIcon)
{
    if (m_hIcon != nullptr)
        DestroyIcon(m_hIcon);
    m_hIcon = hIcon;
    Invalidate();
}

void ClaButtonIcon::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect rcItem = lpDrawItemStruct->rcItem;

    // Draw the button background
    pDC->FillRect(&rcItem, &CBrush(GetSysColor(COLOR_BTNFACE)));

    // Draw the button text
    CString strText;
    GetWindowText(strText);
    pDC->DrawText(strText, rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Draw the button icon
    if (m_hIcon != nullptr)
    {
        int nIconSize = min(rcItem.Width(), rcItem.Height()) - 8;
        int nIconX = rcItem.left + (rcItem.Width() - nIconSize) / 2;
        int nIconY = rcItem.top + (rcItem.Height() - nIconSize) / 2;
        DrawIconEx(pDC->m_hDC, nIconX, nIconY, m_hIcon, nIconSize, nIconSize, 0, nullptr, DI_NORMAL);
    }
}