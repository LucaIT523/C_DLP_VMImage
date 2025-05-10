#pragma once

#include "KWnd.h"

class KMenuItem {
public:
	KMenuItem() {
		isHeader = FALSE;
		memset(wszText, 0, sizeof(wszText));
		m_pIcon = NULL;
		nMsgID = -1;
	}
	~KMenuItem() {
		if (m_pIcon) delete m_pIcon;
	}

	void operator=(KMenuItem& pItem) {
		isHeader = pItem.isHeader;
		wcscpy_s(wszText, 64, pItem.wszText);
		if (pItem.m_pIcon)
			m_pIcon = pItem.m_pIcon->Clone();
		else
			m_pIcon = NULL;
		nMsgID = pItem.nMsgID;
	}

public:
	BOOL isHeader;
	wchar_t wszText[64];
	Gdiplus::Image* m_pIcon;
	UINT nMsgID;
};
// KStaticMenu
class KStaticMenu : public KWnd
{
	DECLARE_DYNAMIC(KStaticMenu)

public:
	KStaticMenu();
	virtual ~KStaticMenu();
	
	bool LoadImageK(UINT imageID, UINT imageIDSel, LPCTSTR lpszResType = _T("PNG"), HINSTANCE hInstance = NULL);

	void _DrawBG(Gdiplus::Graphics& g, Gdiplus::Image* pImage);
	int _getHeight(int p_nIndex);
	void _DrawBody(Gdiplus::Graphics& g, int i);
	virtual void Render(Gdiplus::Graphics& g);

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();

public:
	virtual void UpdateState();

public:
	void addItem(BOOL p_bHeader, const wchar_t* p_wszText, UINT iconID, UINT p_nMsgID);
	CArray< KMenuItem, KMenuItem&> m_lstItems;
	CArray< CRect, CRect&> m_lstRects;

	Gdiplus::Image* m_pCurrImage;
};


