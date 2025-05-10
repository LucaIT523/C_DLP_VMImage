// TranlucentButton.cpp : implementation file
//

#include "pch.h"
#include "KEdit.h"
#include "Utility.h"


// KEdit

IMPLEMENT_DYNAMIC(KEdit, KWnd)

KEdit::KEdit()
	: KWnd()
{
	m_imageList.resize(2, NULL);
	m_pIconImage = NULL;
	m_pImageBtn = NULL;
	_nPaddingLeft = 24;
	_nPaddingLeft1 = 20;
	_wzPwd = 0;
	_bDownIcon = FALSE;
	_bFocus = FALSE;
}

KEdit::~KEdit()
{
	//DestroyImageList();
}

BEGIN_MESSAGE_MAP(KEdit, KWnd)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

// KEdit message handlers

void KEdit::PreSubclassWindow()
{
	KWnd::PreSubclassWindow();
}

void KEdit::UpdateState()
{
	NotifyRender();
}

void KEdit::Render(Gdiplus::Graphics& g)
{
	int PaddingL = _nPaddingLeft;
	int PaddingR = 0;
	CString szText;
	GetWindowText(szText);
	if (1)
	{
		{
			CRect rc;
			GetWindowRect(&rc);
			GetParent()->ScreenToClient(&rc);

			Gdiplus::RectF rect((float)rc.left, (float)rc.top, (float)rc.Width(), (float)rc.Height());
			g.DrawImage(m_pCurrImage, rect);
		}
		_nPaddingRight = 0;
		if (m_pIconImage != NULL) {

			CRect rc;
			GetWindowRect(&rc);
			GetParent()->ScreenToClient(&rc);

			rc.left += PaddingL;
			rc.right = rc.left + m_pIconImage->GetWidth();
			rc.top += (rc.Height() - m_pIconImage->GetHeight()) / 2;
			rc.bottom = rc.top + m_pIconImage->GetHeight();

			Gdiplus::RectF rect((float)rc.left, (float)rc.top, (float)rc.Width(), (float)rc.Height());
			g.DrawImage(m_pIconImage, rect);

			PaddingL += m_pIconImage->GetWidth() + _nPaddingLeft1;
		}
		if (m_pImageBtn != NULL) {

			CRect rc_right;
			GetWindowRect(&rc_right);
			GetParent()->ScreenToClient(&rc_right);

			PaddingR = 20;
			int width = rc_right.Width();
			rc_right.left += rc_right.Width() - m_pImageBtn->GetWidth() - PaddingR;
			rc_right.right = rc_right.right - PaddingR;
			rc_right.top += (rc_right.Height() - m_pImageBtn->GetHeight()) / 2;
			rc_right.bottom = rc_right.top + m_pImageBtn->GetHeight();

			Gdiplus::RectF rect((float)rc_right.left, (float)rc_right.top, (float)rc_right.Width(), (float)rc_right.Height());
			g.DrawImage(m_pImageBtn, rect);

			PaddingR += m_pImageBtn->GetWidth();
		}

		{
			Gdiplus::FontStyle style;
			if (_bold) {
				style = Gdiplus::FontStyleBold;
			}
			else {
				style = Gdiplus::FontStyleRegular;
			}
			Gdiplus::Font m_pFont(_wszFont, _w, style);

			Gdiplus::StringFormat format;

			format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

			if (_align == 0) {
				format.SetAlignment(Gdiplus::StringAlignmentNear);
			}
			else if (_align == 1) {
				format.SetAlignment(Gdiplus::StringAlignmentCenter);
			}
			else {
				format.SetAlignment(Gdiplus::StringAlignmentFar);
			}
			format.SetAlignment(Gdiplus::StringAlignmentNear); // Left-align text
			format.SetLineAlignment(Gdiplus::StringAlignmentCenter); // Vertically center text
			format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
			//format.SetTrimming(Gdiplus::StringTrimmingNone);
			Gdiplus::SolidBrush brush(Gdiplus::Color(GetRValue(_c1), GetGValue(_c1), GetBValue(_c1)));
			if (_bFocus)
				brush.SetColor(Gdiplus::Color(GetRValue(_c1), GetGValue(_c1), GetBValue(_c1)));
			else
				brush.SetColor(Gdiplus::Color(GetRValue(_c), GetGValue(_c), GetBValue(_c)));

			CRect rc;
			GetWindowRect(&rc);
			GetParent()->ScreenToClient(&rc);
			rc.left += PaddingL;
			rc.right -= PaddingR;
			Gdiplus::RectF rect((float)rc.left, (float)rc.top, (float)rc.Width(), (float)rc.Height());
			Gdiplus::RectF textBounds;
			if (_wzPwd != 0 && !_bDownIcon)
			{
				CString strPwd;
				for (int i = 0; i < szText.GetLength(); i++) {
					strPwd = strPwd + _wzPwd;
				}
				g.MeasureString(strPwd, strPwd.GetLength(), &m_pFont, rect, &textBounds);
				g.DrawString(strPwd, strPwd.GetLength(), &m_pFont, rect, &format, &brush);
			}
			else {
				g.MeasureString(szText, szText.GetLength(), &m_pFont, rect, &textBounds);
				g.DrawString(szText, szText.GetLength(), &m_pFont, rect, &format, &brush);
			}
			if (_bFocus) {
				float lastLetterX = rect.X + textBounds.Width; // Right edge of the text
				if (lastLetterX <= 0) {
					lastLetterX = rect.X;
				}
				//float lineTop = textBounds.Y;                        // Top of the text rectangle
				float lineTop = rect.Y + rect.Height / 2 - 9;
				float lineBottom = lineTop + 18;
				Gdiplus::Pen linePen(Gdiplus::Color(GetRValue(_c1), GetGValue(_c1), GetBValue(_c1))); // Black color
				g.DrawLine(&linePen, lastLetterX, lineTop, lastLetterX, lineBottom);
			}
		}
	}
}

bool KEdit::LoadImageK(UINT imageID[2], LPCTSTR lpszResType, HINSTANCE hInstance)
{
	DestroyImageList();

	m_imageList[TWS_NORMAL] = CUtility::LoadImage(imageID[0], lpszResType, hInstance);
	if (m_imageList[TWS_NORMAL] == NULL)
		return false;

	if (imageID[1] != 0)
		m_imageList[1] = CUtility::LoadImage(imageID[1], lpszResType, hInstance);

	m_pCurrImage = m_imageList[0];

	//VERIFY(SetWindowPos(NULL, -1, -1, m_pCurrImage->GetWidth(), m_pCurrImage->GetHeight(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE));
	return true;
}

bool KEdit::SetIconImage(UINT imageID, LPCTSTR lpszResType /*= _T("PNG")*/, HINSTANCE hInstance /*= NULL*/)
{
	m_pIconImage = CUtility::LoadImage(imageID, lpszResType, hInstance);
	return true;
}
bool KEdit::SetRightIcon(UINT imageID, LPCTSTR lpszResType /*= _T("PNG")*/, HINSTANCE hInstance /*= NULL*/, UINT msgID)
{
	m_pImageBtn = CUtility::LoadImage(imageID, lpszResType, hInstance);
	_nMsgID = msgID;
	return true;
}


LRESULT KEdit::OnSetText(WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = Default();
	UpdateState();
	return Result;
}


void KEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	KWnd::OnChar(nChar, nRepCnt, nFlags);

	// TODO: Add your message handler code here and/or call default
	UpdateState();
}


void KEdit::OnSetFocus(CWnd* pOldWnd)
{
	KWnd::OnSetFocus(pOldWnd);
	m_pCurrImage = m_imageList[1];
	_bFocus = TRUE;
	UpdateState();
}


void KEdit::OnKillFocus(CWnd* pNewWnd)
{
	KWnd::OnKillFocus(pNewWnd);
	m_pCurrImage = m_imageList[0];
	_bFocus = FALSE;
	UpdateState();
}


void KEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pImageBtn == NULL) {
		KWnd::OnLButtonDown(nFlags, point);
		return;
	}
	CRect rect;
	GetWindowRect(rect);
	CRect area;
	area.left = rect.Width() - 30 - m_pImageBtn->GetWidth();
	area.right = rect.Width() - 10;
	area.top = 0;
	area.bottom= rect.Height();
	if (point.x >= area.left && point.x <= area.right && point.y >= area.top && point.y <= area.bottom) {
		_bDownIcon = TRUE;
	}

	UpdateState();

	KWnd::OnLButtonDown(nFlags, point);
}


void KEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pImageBtn == NULL) {
		KWnd::OnMouseMove(nFlags, point);
		return;
	}
	CRect rect;
	GetWindowRect(rect);
	CRect area;
	area.left = rect.Width() - 30 - m_pImageBtn->GetWidth();
	area.right = rect.Width() - 10;
	area.top = 0;
	area.bottom = rect.Height();
	if (point.x >= area.left && point.x <= area.right && point.y >= area.top && point.y <= area.bottom) {
		_btn_ready = true;
	} else {
		_btn_ready = false;
		_bDownIcon = FALSE;
	}
	HCURSOR hCursor = ::LoadCursor(NULL, IDC_IBEAM);
	if (_btn_ready) {
		hCursor = ::LoadCursor(NULL, IDC_HAND);
	}
	SetCursor(hCursor);

	UpdateState();
	KWnd::OnMouseMove(nFlags, point);
}


BOOL KEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	//return KWnd::OnSetCursor(pWnd, nHitTest, message);
	return TRUE;
}


void KEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!_bDownIcon) {
		KWnd::OnLButtonUp(nFlags, point);
		return;
	}

	if (_btn_ready) {
		_bDownIcon = FALSE;
	}

	UpdateState();

	if (_nMsgID) {
		GetParent()->SendMessage(_nMsgID);
	}

	KWnd::OnLButtonUp(nFlags, point);
}


void KEdit::OnMouseLeave()
{
	UpdateState();

	if (_btn_ready) {
		_bDownIcon = FALSE;
	}

	KWnd::OnMouseLeave();
}
