// ClaDlgAboutAgent.cpp : implementation file
//

#include "pch.h"
#include "KAgentQR.h"
#include "afxdialogex.h"
#include "ClaDlgAboutAgent.h"


ClaDlgAboutAgent::ClaDlgAboutAgent(UINT p_nBgID, CWnd* pParent /*=nullptr*/)
	: KDialog(IDD_PAGE_ABOUT_US, p_nBgID, L"PNG", NULL, pParent, TRUE)
{

}

ClaDlgAboutAgent::~ClaDlgAboutAgent()
{
}
KDialog* ClaDlgAboutAgent::CreateStandardDialog()
{
	return NULL;
}

void ClaDlgAboutAgent::OnInitChildrenWnds()
{
	UINT nImgIdCloses[TWS_BUTTON_NUM] = { IDB_PNG_BTN_CLOSE, IDB_PNG_BTN_CLOSE_H, 0, 0 };
	m_btnClose.LoadImageList(nImgIdCloses);

	RegisterTranslucentWnd(&m_btnClose);

	m_pngLogo.LoadImageK(IDB_PNG_LOGO);
	RegisterTranslucentWnd(&m_pngLogo);
}

void ClaDlgAboutAgent::DoDataExchange(CDataExchange* pDX)
{
	KDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_btnClose);
	DDX_Control(pDX, IDC_STATIC_LOGO, m_pngLogo);
}


BEGIN_MESSAGE_MAP(ClaDlgAboutAgent, KDialog)
	ON_BN_CLICKED(IDCANCEL, &ClaDlgAboutAgent::OnBnClickedCancel)
END_MESSAGE_MAP()


// ClaDlgAboutAgent message handlers


void ClaDlgAboutAgent::OnBnClickedCancel()
{
	KDialog::OnOK();
	// TODO: Add your control notification handler code here
}


BOOL ClaDlgAboutAgent::OnInitDialog()
{
	KDialog::OnInitDialog();
	CRect screenRect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);

	// Get the dialog dimensions
	CRect dialogRect;
	GetWindowRect(&dialogRect);
	int w = dialogRect.Width(),h = dialogRect.Height();
	// TODO:  Add extra initialization here
	int xPos = 0;
	int yPos = 0;

	xPos = screenRect.right - w;
	yPos = screenRect.bottom - h;


	MoveWindow(xPos, yPos, w, h);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT ClaDlgAboutAgent::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_USER + 2) {
		OnCancel();
	}

	return KDialog::WindowProc(message, wParam, lParam);
}
