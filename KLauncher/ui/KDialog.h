
#pragma once

#include "KWnd.h"
#include <list>

typedef std::list<KWnd*> TranslucentWndList; 

class KDialog : public CDialog, public IRenderListener
{
public:
	KDialog(UINT nIDTemplate, UINT nImgID, LPCTSTR lpszType = _T("PNG"), HINSTANCE hResourceModule = NULL, CWnd* pParent = NULL, BOOL p_bMain = FALSE);
	
	virtual ~KDialog();

	void SetEnableDrag(bool bEnableDrag);
	void SetCenterAligned(bool bCenterAligned);

	virtual KDialog* CreateStandardDialog() = 0;
	virtual void OnInitChildrenWnds() = 0;

	void RegisterTranslucentWnd(KWnd* translucentWnd);
	void UnregisterTranslucentWnd(KWnd* translucentWnd);

	void StartBuffered();
	void EndBuffered();
	void CreateDialog_();
	void ShowDialog(BOOL p_bCallback = TRUE);
	void HideDialog();
	
	virtual void OnRenderEvent(KWnd* translucentWnd);
	virtual void OnShowPage(void) {};

	void UpdateView();

protected:
	BOOL _bMain;
	BOOL _bCreated;
	BLENDFUNCTION m_blend;
	Gdiplus::Image* m_pImage;
	//RECT m_rcWindow;
	bool m_bEnableDrag;
	bool m_bCenterAligned;
	bool m_bBuffered;

	KDialog* m_pStandardDlg;

	TranslucentWndList m_translucentWndList;

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnOK();
	virtual void OnCancel();
	BOOL _bMoveFinish;
public:
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
};
