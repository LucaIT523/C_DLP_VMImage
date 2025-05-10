// TranlucentButton.cpp : implementation file
//

#include "pch.h"
#include "KStaticText.h"
#include "Utility.h"
#include <sstream>

// KStaticText

IMPLEMENT_DYNAMIC(KStaticText, KWnd)

KStaticText::KStaticText()
	: KWnd()
{
}

KStaticText::~KStaticText()
{
	//DestroyImageList();
}

BEGIN_MESSAGE_MAP(KStaticText, KWnd)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

// KStaticText message handlers

void KStaticText::PreSubclassWindow()
{
	KWnd::PreSubclassWindow();
}

void KStaticText::UpdateState()
{
	NotifyRender();
}

LRESULT KStaticText::OnSetText(WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = Default();
	UpdateState();
	return Result;
}


void KStaticText::Render(Gdiplus::Graphics& g)
{
	CString szText;
	GetWindowText(szText);
	if (szText.GetLength() > 0)
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

		if (_alignV == 0) {
			format.SetLineAlignment(Gdiplus::StringAlignmentNear);
		}
		else if (_alignV == 1) {
			format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		}
		else {
			format.SetLineAlignment(Gdiplus::StringAlignmentFar);
		}

		if (_align == 0) {
			format.SetAlignment(Gdiplus::StringAlignmentNear);
		}
		else if (_align == 1) {
			format.SetAlignment(Gdiplus::StringAlignmentCenter);
		}
		else {
			format.SetAlignment(Gdiplus::StringAlignmentFar);
		}

		Gdiplus::SolidBrush brush(Gdiplus::Color(GetRValue(_c), GetGValue(_c), GetBValue(_c)));

		CRect rc;
		GetWindowRect(&rc);
		GetParent()->ScreenToClient(&rc);

		if (_linespace == 0)
		{
			Gdiplus::RectF rect((float)rc.left, (float)rc.top, (float)rc.Width(), (float)rc.Height());
			g.DrawString(szText, szText.GetLength(), &m_pFont, rect, &format, &brush);
		}
		else {
			// Split the text into lines
			std::wstring text(szText);
			std::wistringstream stream(text);
			std::wstring line;
			float y = (float)rc.top; // Starting y position

			while (std::getline(stream, line))
			{
				Gdiplus::RectF rect((float)rc.left, y, (float)rc.Width(), (float)rc.Height());

				// Draw each line with alignment
				g.DrawString(line.c_str(), line.length(), &m_pFont, rect, &format, &brush);

				// Increment y position for next line
				y += _linespace; // Add custom line spacing
			}
		}
	}
}
