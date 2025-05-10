// ClaDlg64.cpp : implementation file
//

#include "pch.h"
#include "launcher.h"
#include "afxdialogex.h"
#include "ClaDlg64.h"
#include "ClaProcess.h"
#include "ClaPathMgr.h"


// ClaDlg64 dialog

IMPLEMENT_DYNAMIC(ClaDlg64, CDialog)

ClaDlg64::ClaDlg64(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_64, pParent)
{

}

ClaDlg64::~ClaDlg64()
{
}

void ClaDlg64::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ClaDlg64, CDialog)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


typedef unsigned int (*FN_KC_HOOK_SET)();
typedef unsigned int (*FN_KC_HOOK_UNSET)();

// ClaDlg64 message handlers
UINT TF_PROC(void* p) {
	ClaDlg64* pThis = (ClaDlg64*)p;

	DWORD dwParentProcID = ClaProcess::GetParentProcessId(0);
	HANDLE hProcParent = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwParentProcID);
	wchar_t wszDllPath[MAX_PATH]; memset(wszDllPath, 0, sizeof(wszDllPath));
	swprintf_s(wszDllPath, MAX_PATH, L"%s\\KenanHelperVM64.dll", ClaPathMgr::GetDP());
	int nVmxID = -1;


	while (TRUE) {
		if (WaitForSingleObject(hProcParent, 500) != WAIT_TIMEOUT) {
			break;
		}

		int nPID = ClaProcess::FindProcessID(L"vmware-vmx.exe");
		if (nPID <= 0) continue;

		if (nVmxID != nPID) {
			ClaProcess::InjectDLL(nPID, wszDllPath);
			nVmxID = nPID;
		}
	}

	pThis->PostMessageW(WM_USER + 1);
	return 0;
}

FN_KC_HOOK_UNSET kc_hook_unset;

BOOL ClaDlg64::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strCmd = GetCommandLine();
	if (strCmd.Find(L"--pdnd") < 0) {
		EndDialog(IDCANCEL);
		return TRUE;
	}

	HMODULE hMod = LoadLibrary(L"KenanHelperVM64.dll");
	FN_KC_HOOK_SET kc_hook_set = (FN_KC_HOOK_SET)GetProcAddress(hMod, "KC_HOOK_SET");
	kc_hook_unset = (FN_KC_HOOK_SET)GetProcAddress(hMod, "KC_HOOK_UNSET");
	if (kc_hook_set != NULL) {
		kc_hook_set();
	}


	DWORD dwTID = 0;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_PROC, this, 0, &dwTID);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT ClaDlg64::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_USER + 1) {

		OnCancel();
	}

	return CDialog::WindowProc(message, wParam, lParam);
}


void ClaDlg64::OnCancel()
{
	if (kc_hook_unset != NULL) {
		kc_hook_unset();
	}

	CDialog::OnCancel();
}


void ClaDlg64::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	lpwndpos->flags &= ~SWP_SHOWWINDOW;
	CDialog::OnWindowPosChanging(lpwndpos);
}
