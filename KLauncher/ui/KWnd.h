#pragma once

#include <vector>

class KWnd;

struct IRenderListener 
{
	virtual ~IRenderListener() { }
	virtual void OnRenderEvent(KWnd* translucentWnd) = 0;
};

enum TranslucentWndState
{
	TWS_NORMAL = 0,
	//TWS_FOCUSED,
	TWS_MOUSEOVER,
	TWS_PRESSED,
	TWS_DISABLED,
	TWS_SELECTED,

	TWS_BUTTON_NUM = TWS_SELECTED + 1,
	TWS_FORCE_DWORD = 0x7fffffff,
};
enum TranslucentWndComboState
{
	TWS_COMBO_NORMAL = 0,
	TWS_COMBO_ACTIVE,
	TWS_COMBO_FIRST_NORMAL,
	TWS_COMBO_FIRST_HOVER,
	TWS_COMBO_LAST_NORMAL,
	TWS_COMBO_LAST_HOVER,
	TWS_COMBO_MIDDLE_NORMAL,
	TWS_COMBO_MIDDLE_HOVER,

	TWS_COMBO_NUM = TWS_COMBO_MIDDLE_HOVER + 1,
	//TWS_FORCE_DWORD = 0x7fffffff,
};

// KWnd

class KWnd : public CWnd
{
	DECLARE_DYNAMIC(KWnd)

public:
	KWnd();
	virtual ~KWnd();
	
	virtual void Render(Gdiplus::Graphics& g) = 0;

	BOOL ShowWindow(int nCmdShow);

	void SetRenderListener(IRenderListener* pRenderListener) { m_pRenderListener = pRenderListener; }
	void NotifyRender();

	void DestroyImageList();

	void DestroyImage(UINT index);

	bool IsDisabled() const { return m_bDisabled; }
	bool IsVisible() const { return m_bVisible; } 

protected:
	virtual void UpdateState();
	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnable(BOOL bEnable);

protected:
	typedef std::vector<Gdiplus::Image*> ImageList;
	ImageList m_imageList;

	IRenderListener* m_pRenderListener;
	bool m_bDisabled;
	bool m_bVisible;
};


