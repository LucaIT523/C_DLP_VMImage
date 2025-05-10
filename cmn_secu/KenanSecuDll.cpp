
#include "pch.h"
#include "KenanSecuDll.h"

FN_SECU_CREATE_PROCESS_WITH_DLL SECU_CreateProcessWithDll = NULL;
FN_SECU_FINISH SECU_Finish = NULL;
FN_SECU_HOOK SECU_SetHook = NULL;
FN_SECU_UNHOOK SECU_UnsetHook = NULL;

unsigned int SECU_Init()
{
#ifdef _WIN64
	HMODULE hMod = LoadLibrary(L"KenanSecu64.dll");
#else
	HMODULE hMod = LoadLibrary(L"KenanSecu32.dll");
#endif
	if (hMod == NULL) return -1;

	SECU_CreateProcessWithDll = (FN_SECU_CREATE_PROCESS_WITH_DLL)GetProcAddress(hMod, "SECU_CREATE_PROCESS_WITH_DLL");
	SECU_Finish = (FN_SECU_FINISH)GetProcAddress(hMod, "SECU_FINISH");
	SECU_SetHook   = (FN_SECU_HOOK)GetProcAddress(hMod  , "SetGlobalHook");
	SECU_UnsetHook = (FN_SECU_UNHOOK)GetProcAddress(hMod, "UnsetGlobalHook");

	if (SECU_CreateProcessWithDll == NULL) {
		return -2;
	}

	return 0;
}
