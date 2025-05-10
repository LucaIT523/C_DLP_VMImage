#pragma once

#include "KWnd.h"

// KStaticPng
class KStaticPng : public KWnd
{
	DECLARE_DYNAMIC(KStaticPng)

public:
	KStaticPng();
	virtual ~KStaticPng();
	
	bool LoadImageK(UINT imageID, LPCTSTR lpszResType = _T("PNG"), HINSTANCE hInstance = NULL);

	virtual void Render(Gdiplus::Graphics& g);

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();

public:
	virtual void UpdateState();

	Gdiplus::Image* m_pCurrImage;
};


