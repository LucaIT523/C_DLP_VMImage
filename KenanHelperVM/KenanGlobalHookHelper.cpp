#include "pch.h"
#include "KenanHelperVM.h"
#include "KenanGlobalHookHelper.h"

HHOOK g_hHookKeyboard = NULL;
HHOOK g_hHookMouse = NULL;
HHOOK g_hHookKeyboardLL = NULL;
HHOOK g_hHookMouseLL = NULL;
HHOOK g_hHookProc = NULL;

extern "C" __declspec(dllexport) LRESULT CALLBACK HookProcKeyboard(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		// Process the hook message here
	}
	return CallNextHookEx(g_hHookKeyboard, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport) LRESULT CALLBACK HookProcMouse(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		// Process the hook message here
	}
	return CallNextHookEx(g_hHookMouse, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport) LRESULT CALLBACK HookProcKeyboardLL(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		// Process the hook message here
	}
	return CallNextHookEx(g_hHookKeyboardLL, nCode, wParam, lParam);
}
extern "C" __declspec(dllexport) LRESULT CALLBACK HookProcMouseLL(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		// Process the hook message here
	}
	return CallNextHookEx(g_hHookMouseLL, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport) unsigned int KC_HOOK_SET() {
	if (g_hHookKeyboard == NULL) {
		g_hHookKeyboard = SetWindowsHookEx(WH_KEYBOARD, HookProcKeyboard, g_hInstance, 0);
	}
	if (g_hHookMouse == NULL) {
		g_hHookMouse = SetWindowsHookEx(WH_MOUSE, HookProcMouse, g_hInstance, 0);
	}
	if (g_hHookKeyboardLL == NULL) {
		g_hHookKeyboardLL = SetWindowsHookEx(WH_KEYBOARD_LL, HookProcKeyboardLL, NULL, 0);
	}
	if (g_hHookMouseLL == NULL) {
		g_hHookMouseLL = SetWindowsHookEx(WH_MOUSE_LL, HookProcMouseLL, NULL, 0);
	}

	if (g_hHookKeyboard == NULL || g_hHookMouse == NULL) {
		return -1;
	}
	return 0;
}

extern "C" __declspec(dllexport) unsigned int KC_HOOK_UNSET() {
	if (g_hHookKeyboard) {
		UnhookWindowsHookEx(g_hHookKeyboard);
		g_hHookKeyboard = NULL;
	}
	if (g_hHookMouse) {
		UnhookWindowsHookEx(g_hHookMouse);
		g_hHookMouse = NULL;
	}
	if (g_hHookKeyboardLL) {
		UnhookWindowsHookEx(g_hHookKeyboardLL);
		g_hHookKeyboardLL = NULL;
	}
	if (g_hHookMouseLL) {
		UnhookWindowsHookEx(g_hHookMouseLL);
		g_hHookMouseLL = NULL;
	}
	return 0;
}