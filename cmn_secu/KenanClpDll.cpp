
#include "pch.h"
#include "KenanClpDll.h"

FN_HOOK		CLP_SetHook = NULL;
FN_UNHOOK	CLP_UnsetHook = NULL; 

unsigned int CLP_Init()
{
#ifdef _WIN64
	HMODULE hMod = LoadLibrary(L"KenanClp64.dll");
#else//_WIN64
	HMODULE hMod = LoadLibrary(L"KenanClp32.dll");
#endif//_WIN64
	if (hMod == NULL) {
		return -1;
	}

	CLP_SetHook = (FN_HOOK)GetProcAddress(hMod, "SetGlobalHook");
	CLP_UnsetHook = (FN_UNHOOK)GetProcAddress(hMod, "UnsetGlobalHook");

	if (CLP_SetHook == NULL || CLP_UnsetHook == NULL) {
		return -2;
	}

	return 0;
}
