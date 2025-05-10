// ClaPageAboutUS.cpp : implementation file
//

#include "pch.h"
#include "KLauncher.h"
#include "KLauncherDlg.h"
#include "afxdialogex.h"
#include "ClaPageAboutUS.h"
#include "KGlobal.h"

// ClaPageAboutUS dialog

ClaPageAboutUS::ClaPageAboutUS(CWnd* pParent)
	: KDialog(IDD_PAGE_ABOUT_US, 0, L"PNG", 0, pParent, FALSE)
{

}

ClaPageAboutUS::~ClaPageAboutUS()
{
}

void ClaPageAboutUS::DoDataExchange(CDataExchange* pDX)
{
	KDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_LOGO, m_pngLogo);
	DDX_Control(pDX, IDC_BTN_BACK, m_btnBack);
	DDX_Control(pDX, IDC_STATIC_BG, m_background);
}


BEGIN_MESSAGE_MAP(ClaPageAboutUS, KDialog)
	ON_BN_CLICKED(IDC_BTN_BACK, &ClaPageAboutUS::OnBnClickedBtnBack)
END_MESSAGE_MAP()


// ClaPageAboutUS message handlers


void ClaPageAboutUS::OnBnClickedBtnBack()
{
	g_pMain->ShowMainPage(FALSE);
	// TODO: Add your control notification handler code here
}
void ClaPageAboutUS::OnInitChildrenWnds() {
	m_background.LoadImageK(IDB_PNG_ABOUT_US_BG);
	RegisterTranslucentWnd(&m_background);
	m_pngLogo.LoadImageK(IDB_PNG_LOGO);
	RegisterTranslucentWnd(&m_pngLogo);
	UINT nImgIdBack[TWS_BUTTON_NUM] = { IDB_PNG_BTN_BACK, IDB_PNG_BTN_BACK, IDB_PNG_BTN_BACK, 0 };
	m_btnBack.LoadImageList(nImgIdBack);
	RegisterTranslucentWnd(&m_btnBack);
	UpdateView();
}

