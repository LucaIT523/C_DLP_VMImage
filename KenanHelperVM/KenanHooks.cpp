#include "pch.h"
#include "KenanHelperVM.h"
#include "KenanHoos.h"
#include "KenanSecuProc.h"
#include "KenanCliphelper.h"
#include "KenanVmxHelper.h"
#include "KenanVmxRedirector.h"
#include "KenanBoundary.h"

void init_process_vmhelper()
{
	hook_vmx();
}

void init_process_nonsecure()
{
	hook_clipboard();
	hook_clipboard_vmx();
}

void init_process_secure()
{
	hook_redirector();

	hook_clipboard();

	create_thread_capturesafe();

	startDrawBoundary(g_hInstance);

	start_watch_parent();
}

void init_process_manager()
{
	//start_draw_boundary();
	hook_redirector();
}

void init_hooks()
{
	if (wcsstr(g_wszAppPath, L"VMPLAYER.EXE") != NULL) {
		g_isSecuProc = TRUE;
	}
	else {
		g_isSecuProc = FALSE;
	}
	if (wcsstr(g_wszAppPath, L"VMWARE-VMX.EXE") != NULL) {
		g_isVmxProc = TRUE;
	}
	if (wcsstr(g_wszAppPath, L"LAUNCHER32.EXE") != NULL) {
		g_isMgrProc = TRUE;
	}

	if (g_isMgrProc) {
		init_process_manager();
	}else if (g_isVmxProc) {
		init_process_vmhelper();
	}else if (g_isSecuProc) {
		init_process_secure();
	}
	else {
		init_process_nonsecure();
	}
}

void uninit_hooks()
{
	unhook_clipboard();
	unhook_vmx();
	unhook_redirector();

	terminate_thread_capturesafe();
	stopDrawBoundary();
	unhook_clipboard_vmx();
}

DWORD WINAPI tf_watch_parent(void*) {
	HANDLE hParentProc = OpenProcess(SYNCHRONIZE, FALSE, g_nParentProcID);
	WaitForSingleObject(hParentProc, INFINITE);
	DeleteFile(g_wszVML);
	TerminateProcess(GetCurrentProcess(), 0);
	return 0;
}

void start_watch_parent() {
	DWORD dwTID;
	CreateThread(NULL, 0, tf_watch_parent, NULL, 0, &dwTID);
}