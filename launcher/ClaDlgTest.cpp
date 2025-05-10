// ClaDlgTest.cpp : implementation file
//

#include "pch.h"
#include "launcher.h"
#include "afxdialogex.h"
#include "ClaDlgTest.h"


// ClaDlgTest dialog

IMPLEMENT_DYNAMIC(ClaDlgTest, CDialogEx)

ClaDlgTest::ClaDlgTest(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_TEST, pParent)
	, m_strTest(_T(""))
	, m_strPass(_T(""))
{

}

ClaDlgTest::~ClaDlgTest()
{
}

void ClaDlgTest::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strTest);
	DDX_Text(pDX, IDC_EDIT2, m_strPass);
}


BEGIN_MESSAGE_MAP(ClaDlgTest, CDialogEx)
	ON_BN_CLICKED(IDOK, &ClaDlgTest::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &ClaDlgTest::OnBnClickedButton1)
END_MESSAGE_MAP()


// ClaDlgTest message handlers


void ClaDlgTest::OnBnClickedOk()
{
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


void ClaDlgTest::OnBnClickedButton1()
{
	UpdateData(TRUE);
	CFileDialog dlg(TRUE, L"vmx", NULL, 6, L"VMWare image file|*.vmx|All Files|*.*||");
	if (dlg.DoModal() == IDOK) {
		m_strTest = dlg.GetPathName();
		UpdateData(FALSE);
	}
}
