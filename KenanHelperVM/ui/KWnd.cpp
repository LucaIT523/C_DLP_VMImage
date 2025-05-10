// TranslucentWnd.cpp : implementation file
//

#include "pch.h"
#include "KWnd.h"


// KWnd

IMPLEMENT_DYNAMIC(KWnd, CWnd)
KWnd::KWnd()
	: CWnd()
	, m_pRenderListener(NULL)
	, m_bVisible(true)
	, m_bDisabled(false)
{
}

KWnd::~KWnd()
{
	DestroyImageList();
}


BEGIN_MESSAGE_MAP(KWnd, CWnd)
	ON_WM_ENABLE()
END_MESSAGE_MAP()

// KWnd message handlers
void KWnd::OnEnable(BOOL bEnable)
{
	CWnd::OnEnable(bEnable);

	bool bDisabled = (bEnable != TRUE);
	if (bDisabled != m_bDisabled)
	{
		m_bDisabled = bDisabled;
		UpdateState();
	}
}

BOOL KWnd::ShowWindow(int nCmdShow)
{
	BOOL bPrevious = CWnd::ShowWindow(nCmdShow);
	bool visible = (GetWindowLong(GetSafeHwnd(), GWL_STYLE) & WS_VISIBLE) != 0;
	if (visible != m_bVisible)
	{
		m_bVisible = visible;
		NotifyRender();
	}

	return bPrevious;
}

void KWnd::UpdateState()
{
	NotifyRender();
}

void KWnd::NotifyRender()
{
	if (m_pRenderListener != NULL)
		m_pRenderListener->OnRenderEvent(this);
}

void KWnd::DestroyImageList()
{
	size_t nSize = m_imageList.size();
	for (size_t i = 0; i < nSize; ++i)
	{
		if (m_imageList[i] != NULL)
		{
//			Gdiplus::DllExports::GdipFree(m_imageList[i]);
			m_imageList[i] = NULL;
		}
	}
}

void KWnd::DestroyImage(UINT index)
{
	if (index < m_imageList.size())
	{
		if (m_imageList[index] != NULL)
		{
			Gdiplus::DllExports::GdipFree(m_imageList[index]);
			m_imageList[index] = NULL;
		}
	}
}

void KWnd::PreSubclassWindow()
{
	LONG style = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
	m_bVisible = (style & WS_VISIBLE) != 0;
	m_bDisabled = (style & WS_DISABLED) != 0;
}
