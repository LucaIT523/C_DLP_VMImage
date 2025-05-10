// ClaDlgTrial.cpp : implementation file
//

#include "pch.h"
#include "KLauncher.h"
#include "afxdialogex.h"
#include "ClaDlgTrial.h"
#include "framework.h"
#include "KGlobal.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_USER_CONN_VALIDATE	(WM_USER+0x1001)


ClaDlgTrial::ClaDlgTrial(UINT p_nBgID, CWnd* pParent /*=nullptr*/)
	: KDialog(IDD_KLAUNCHER_TRIAL_DIALOG, p_nBgID, L"PNG", NULL, pParent, TRUE)
	//: KDialog(IDD_KLAUNCHER_DIALOG, pParent)
{
	m_current = 0;
	m_description_list[0] = "Step1: set your server settings";
	m_description_list[1] = "Step2: Login and activate your machine";
	m_description_list[2] = "Step3: download and launch your VM";
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ClaDlgTrial::DoDataExchange(CDataExchange* pDX)
{
	KDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_btnClose);
	DDX_Control(pDX, IDC_DESCRIPTION, m_description);
	DDX_Control(pDX, IDC_BTN_SKIP, m_btn_skip);
	DDX_Control(pDX, IDC_BTN_NEXT, m_btn_next);
	DDX_Control(pDX, IDC_TRIAL_IMAGE, m_trial_img);
	DDX_Control(pDX, IDC_BTN_TRIAL_IMG, m_btn_trial);
	DDX_Control(pDX, IDC_BTN_RADIO, m_radio);
	DDX_Control(pDX, IDC_BTN_FINISH, m_btn_ok);
}

BEGIN_MESSAGE_MAP(ClaDlgTrial, KDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_NEXT, &ClaDlgTrial::OnBnClickedBtnNext)
	ON_BN_CLICKED(IDC_BTN_FINISH, &ClaDlgTrial::OnBnClickedBtnFinish)
	ON_BN_CLICKED(IDC_BTN_SKIP, &ClaDlgTrial::OnBnClickedBtnSkip)
END_MESSAGE_MAP()


// CKLauncherDlg message handlers

BOOL ClaDlgTrial::OnInitDialog()
{
	KDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void ClaDlgTrial::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		KDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR ClaDlgTrial::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

KDialog* ClaDlgTrial::CreateStandardDialog()
{
	return NULL;
}

void ClaDlgTrial::OnInitChildrenWnds()
{
	UINT nImgIdCloses[TWS_BUTTON_NUM] = { IDB_PNG_BTN_CLOSE, IDB_PNG_BTN_CLOSE_H, 0, 0 };
	m_btnClose.LoadImageList(nImgIdCloses);

	RegisterTranslucentWnd(&m_btnClose);

	m_description.setFont(L"Arial", 9, FALSE, RGB(255, 255, 255));
	m_description.setAlign(1);
	m_description.SetWindowTextW(m_description_list[0]);
	RegisterTranslucentWnd(&m_description);

	UINT nImgIdBack[TWS_BUTTON_NUM] = { IDB_PNG_BTN_ICON_SKIP, IDB_PNG_BTN_ICON_SKIP_H, 0, 0, 0 };
	m_btn_skip.LoadImageList(nImgIdBack);
	RegisterTranslucentWnd(&m_btn_skip);

	UINT nImgIdSave[TWS_BUTTON_NUM] = { IDB_PNG_BTN_ICON_NEXT, IDB_PNG_BTN_ICON_NEXT_H, 0, 0, 0 };
	m_btn_next.LoadImageList(nImgIdSave);
	RegisterTranslucentWnd(&m_btn_next);
	
	UINT nImgIdBtn[TWS_BUTTON_NUM] = { IDB_PNG_BTN_ICON_OK, IDB_PNG_BTN_ICON_OK_H, 0, 0, 0 };
	m_btn_ok.LoadImageList(nImgIdBtn);
	RegisterTranslucentWnd(&m_btn_ok);

	m_btn_ok.ShowWindow(SW_HIDE);

	UINT nImgIdOKBtn[TWS_BUTTON_NUM] = { IDB_PNG_TRIAL_SETTING, IDB_PNG_TRIAL_LOGIN, IDB_PNG_TRIAL_MAIN, 0 };
	m_btn_trial.LoadImageList(nImgIdOKBtn);
	RegisterTranslucentWnd(&m_btn_trial);

	UINT nImgIdRadio[TWS_BUTTON_NUM] = { IDB_PNG_STEP1, IDB_PNG_STEP2, IDB_PNG_STEP3, 0 };
	m_radio.LoadImageList(nImgIdRadio);
	RegisterTranslucentWnd(&m_radio);
}


void ClaDlgTrial::OnDestroy()
{

	KDialog::OnDestroy();
}


void ClaDlgTrial::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	CDialog::OnCancel();

	//KDialog::OnCancel();
}

void ClaDlgTrial::OnBnClickedBtnMin()
{
	ShowWindow(SW_MINIMIZE);
	//ShowWindow(SW_MINIMIZE);
}


void ClaDlgTrial::OnBnClickedBtnNext()
{
	if (m_current < 2) {
		m_current++;
		m_description.SetWindowTextW(m_description_list[m_current]);
		m_btn_trial.UpdateState(m_current);
		m_radio.UpdateState(m_current);
		if (m_current == 2) {
			m_btn_ok.ShowWindow(SW_SHOW);
			m_btn_skip.ShowWindow(SW_HIDE);
			m_btn_next.ShowWindow(SW_HIDE);
		}
	}
	// TODO: Add your control notification handler code here
}


void ClaDlgTrial::OnBnClickedBtnFinish()
{
	KDialog::OnOK();
	// TODO: Add your control notification handler code here
}


void ClaDlgTrial::OnBnClickedBtnSkip()
{
	KDialog::OnOK();
	// TODO: Add your control notification handler code here
}
