//

#include "pch.h"
#include "KDialog.h"
#include "Utility.h"
#include <algorithm>

KDialog::KDialog(UINT nIDTemplate, UINT nImgID, LPCTSTR lpszType/* = _T("PNG")*/, HINSTANCE hResourceModule/* = NULL*/, CWnd* pParent /*=NULL*/, BOOL p_bMain /*= FALSE*/)
	: CDialog(nIDTemplate, pParent)
	, m_pStandardDlg(NULL)
	, m_bEnableDrag(true)
	, m_bCenterAligned(true)
	, m_bBuffered(false)
	, _bMain(p_bMain)
	, _bCreated(FALSE)
{
	if (nImgID) {
		m_pImage = CUtility::LoadImage(nImgID, lpszType, hResourceModule);
		VERIFY(m_pImage != NULL);
	}
	else {
		m_pImage = NULL;
	}
	_bMoveFinish = FALSE;
}

KDialog::~KDialog()
{
	//if (m_pStandardDlg != NULL)
	//{
	//	::delete m_pStandardDlg;
	//	m_pStandardDlg = NULL;
	//}

//	DllExports::GdipFree(m_pImage);
	m_pImage = NULL;
}

BEGIN_MESSAGE_MAP(KDialog, CDialog)
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
//	ON_WM_SHOWWINDOW()
ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

void KDialog::SetEnableDrag(bool bEnableDrag)
{
	m_bEnableDrag = bEnableDrag;
}

void KDialog::SetCenterAligned(bool bCenterAligned)
{
	m_bCenterAligned = bCenterAligned;
}

void KDialog::RegisterTranslucentWnd(KWnd* translucentWnd)
{
	translucentWnd->SetRenderListener(this);
	m_translucentWndList.push_back(translucentWnd);
}

void KDialog::UnregisterTranslucentWnd(KWnd* translucentWnd)
{
	translucentWnd->SetRenderListener(NULL);

	TranslucentWndList::iterator it = std::find(m_translucentWndList.begin(), m_translucentWndList.end(), translucentWnd);
	if (it != m_translucentWndList.end())
		m_translucentWndList.erase(it);
}

BOOL KDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	//RECT rc;
	//GetWindowRect(&rc);

	// must be WS_POPUP
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	VERIFY((dwStyle & WS_POPUP) != 0);
	VERIFY((dwStyle & WS_CHILD) == 0);

	m_blend.BlendOp = 0;
	m_blend.BlendFlags = 0;
	m_blend.AlphaFormat = 1;
	m_blend.SourceConstantAlpha = 255;//AC_SRC_ALPHA
	
	OnInitChildrenWnds();

	SetEnableDrag(FALSE);

	UpdateView();

	m_pStandardDlg = CreateStandardDialog();
	if (m_pStandardDlg != NULL)
		m_pStandardDlg->ShowDialog();


	return TRUE;
}

BOOL KDialog::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void KDialog::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	if (m_pStandardDlg != NULL)
	{
		::SetWindowPos(m_pStandardDlg->GetSafeHwnd(), NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		m_pStandardDlg->ShowWindow(SW_SHOW);
		_bMoveFinish = TRUE;
	}
}

void KDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_pStandardDlg != NULL)
	{
		::SetWindowPos(m_pStandardDlg->GetSafeHwnd(), NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE);
	}
}

int KDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// Must be overlapped
	//VERIFY((lpCreateStruct->style & WS_POPUP) == 0);
	//VERIFY((lpCreateStruct->style & WS_CHILD) == 0);

	long nX = lpCreateStruct->x;
	long nY = lpCreateStruct->y;
	long nWidth = lpCreateStruct->cx;
	long nHeight = lpCreateStruct->cy;
	CRect rcWindow;

	if (m_bCenterAligned)
	{
		
		if (m_pImage) {
			rcWindow = { (LONG)m_pImage->GetWidth(), (LONG)m_pImage->GetHeight() };
		}
		else {
			CRect rt;
			if (GetParent() != NULL)
				GetParent()->GetClientRect(rt);
			else
				GetClientRect(rt);
			rcWindow = { (LONG)rt.Width(), (LONG)rt.Height() };
		}
		::AdjustWindowRect(&rcWindow, (DWORD)GetWindowLong(m_hWnd, GWL_STYLE), FALSE);
		nWidth = rcWindow.right - rcWindow.left;
		nHeight = rcWindow.bottom - rcWindow.top;
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWindow, 0);
		nX = rcWindow.left + (rcWindow.right - rcWindow.left - nWidth) / 2;
		nY = rcWindow.top + (rcWindow.bottom - rcWindow.top - nHeight) / 2;
	}

	nWidth = (LONG)rcWindow.Width();
	nHeight = (LONG)rcWindow.Height();

	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(WS_CAPTION, WS_POPUP, 0);
	MoveWindow(nX, nY, nWidth, nHeight, TRUE);

	return 0;
}

void KDialog::UpdateView()
{
	CRect rt; GetClientRect(rt);
	SIZE sizeWindow;
	if (m_pImage) {
		//sizeWindow = { (LONG)m_pImage->GetWidth(), (LONG)m_pImage->GetHeight() };
		sizeWindow = { rt.Width(), rt.Height() };
	}
	else {
		CRect rt;
		if (GetParent() != NULL)
			GetParent()->GetClientRect(rt);
		else
			GetClientRect(rt);

		sizeWindow = { (LONG)rt.Width(), (LONG)rt.Height()};
	}

	DWORD dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	if ((dwExStyle & 0x80000) != 0x80000)
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle | 0x80000);

	HDC hDC = ::GetDC(m_hWnd);
	HDC hdcMemory = CreateCompatibleDC(hDC);

	HBITMAP hBitMap = CreateCompatibleBitmap(hDC, sizeWindow.cx, sizeWindow.cy);
	::SelectObject(hdcMemory, hBitMap);

	RECT rcWindow;
	GetWindowRect(&rcWindow);

	BITMAPINFOHEADER stBmpInfoHeader = { 0 };   
	int nBytesPerLine = ((sizeWindow.cx * 32 + 31) & (~31)) >> 3;
	stBmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);   
	stBmpInfoHeader.biWidth = sizeWindow.cx;   
	stBmpInfoHeader.biHeight = sizeWindow.cy;   
	stBmpInfoHeader.biPlanes = 1;   
	stBmpInfoHeader.biBitCount = 32;   
	stBmpInfoHeader.biCompression = BI_RGB;   
	stBmpInfoHeader.biClrUsed = 0;   
	stBmpInfoHeader.biSizeImage = nBytesPerLine * sizeWindow.cy;   

	PVOID pvBits = NULL;   
	HBITMAP hbmpMem = ::CreateDIBSection(NULL, (PBITMAPINFO)&stBmpInfoHeader, DIB_RGB_COLORS, &pvBits, NULL, 0);
	if (hbmpMem == NULL) {
		::DeleteDC(hdcMemory);
		::DeleteDC(hDC);
		return;
	}
	ASSERT(hbmpMem != NULL);

	memset( pvBits, 0, sizeWindow.cx * 4 * sizeWindow.cy);
	if(hbmpMem)   
	{   
		HGDIOBJ hbmpOld = ::SelectObject( hdcMemory, hbmpMem);

		//POINT ptWinPos = { m_rcWindow.left, m_rcWindow.top };
		POINT ptWinPos = { rcWindow.left, rcWindow.top };
		Gdiplus::Graphics graph(hdcMemory);
		graph.SetSmoothingMode(Gdiplus::SmoothingModeNone);

		if (0)//(m_pImage)
			graph.DrawImage(m_pImage, 0, 0, sizeWindow.cx, sizeWindow.cy);
		else {
			Pen pen(Color(255, 21, 54, 119), 8.0f); // GREEN color, 2 pixels width
			// Draw a rectangle (x, y, width, height)
			graph.DrawRectangle(&pen, 0, 0, sizeWindow.cx, sizeWindow.cy);

			//Gdiplus::SolidBrush brush(Color(255, 0, 0));
			//graph.FillRectangle(&brush, 0, 0, sizeWindow.cx, sizeWindow.cy);
		}

		TranslucentWndList::const_iterator it;
		for (it = m_translucentWndList.begin(); it != m_translucentWndList.end(); ++it)
		{
			KWnd* translucentWnd = *it;
			if (translucentWnd->IsVisible())
				translucentWnd->Render(graph);
		}

		HMODULE hFuncInst = LoadLibrary(_T("User32.DLL"));
		typedef BOOL (WINAPI *MYFUNC)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);          
		MYFUNC UpdateLayeredWindow;
		UpdateLayeredWindow = (MYFUNC)::GetProcAddress(hFuncInst, "UpdateLayeredWindow");
		POINT ptSrc = { 0, 0};
		UpdateLayeredWindow(m_hWnd, hDC, &ptWinPos, &sizeWindow, hdcMemory, &ptSrc, 0, &m_blend, 2);

		graph.ReleaseHDC(hdcMemory);
		::SelectObject( hdcMemory, hbmpOld);   
		::DeleteObject(hbmpMem); 
	}

	::DeleteDC(hdcMemory);
	::DeleteDC(hDC);
}

void KDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bEnableDrag)
	{
		::SendMessage( GetSafeHwnd(), WM_SYSCOMMAND, 0xF012, 0);
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void KDialog::OnRenderEvent(KWnd* translucentWnd)
{
	if (!m_bBuffered)
		UpdateView();
}

void KDialog::StartBuffered()
{
	if (m_bBuffered)
		EndBuffered();

	m_bBuffered = true;
}

void KDialog::EndBuffered()
{
	if (m_bBuffered)
	{
		UpdateView();
		m_bBuffered = false;
	}
}

void KDialog::CreateDialog_()
{
	if (_bCreated == FALSE) {
		Create(m_lpszTemplateName, m_pParentWnd);
		_bCreated = TRUE;
	}
	//ShowWindow(SW_SHOW);
}

void KDialog::ShowDialog(BOOL p_bCallback /*= TRUE*/)
{
	CRect rt;
	GetParent()->GetWindowRect(rt);
	::SetWindowPos(GetSafeHwnd(), NULL, rt.left, rt.top, rt.Width(), rt.Height(), SWP_NOZORDER | SWP_NOSIZE);

	if (p_bCallback)
		OnShowPage();

	if (((KDialog*)GetParent())->_bMoveFinish)
		ShowWindow(SW_SHOW);
}

void KDialog::HideDialog()
{
	ShowWindow(SW_HIDE);
}

void KDialog::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	if (_bMain)
		CDialog::OnOK();
}


void KDialog::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	if (_bMain)
		CDialog::OnOK();
}


void KDialog::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanged(lpwndpos);

	// TODO: Add your message handler code here
	UpdateView();
}
