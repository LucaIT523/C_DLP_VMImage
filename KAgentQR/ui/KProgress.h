#pragma once

#include "KWnd.h"

// KProgress
class KProgress : public KWnd
{
	DECLARE_DYNAMIC(KProgress)

public:
	KProgress();
	virtual ~KProgress();
	
	virtual void Render(Gdiplus::Graphics& g);
	void setPosition(int p_nPosition);
	void setRange(int p_nMin, int p_nMax);
	bool SetImageBackground(UINT imageID, LPCTSTR lpszResType = _T("PNG"), HINSTANCE hInstance = NULL);
	bool SetImageFilled(UINT imageID, LPCTSTR lpszResType = _T("PNG"), HINSTANCE hInstance = NULL);
	Gdiplus::Image* m_pBackground;
	Gdiplus::Image* m_pFilled;
protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	virtual void UpdateState();
};


