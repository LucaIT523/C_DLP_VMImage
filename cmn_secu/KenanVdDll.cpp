
#include "pch.h"
#include "KenanVdDll.h"

FN_VD_CREATE		VD_Create = NULL;
FN_VD_MOUNT_INIT	VD_MountInit = NULL;
FN_VD_MOUNT			VD_Mount = NULL;
FN_VD_WAIT			VD_Wait = NULL;

unsigned int VD_Init()
{
#ifdef _WIN64
	HMODULE hMod = LoadLibrary(L"KenanVDMgr64.dll");
#else
	HMODULE hMod = LoadLibrary(L"KenanVDMgr32.dll");
#endif // _WIN64
	if (hMod == NULL) return -1;

	VD_Create = (FN_VD_CREATE)GetProcAddress(hMod, "VD_CREATE");
	VD_MountInit = (FN_VD_MOUNT_INIT)GetProcAddress(hMod, "VD_MOUNT_INIT");
	VD_Mount = (FN_VD_MOUNT)GetProcAddress(hMod, "VD_MOUNT");
	VD_Wait = (FN_VD_WAIT)GetProcAddress(hMod, "VD_WAIT");

	if (VD_Create == NULL || VD_MountInit == NULL || VD_Mount == NULL) {
		return -2;
	}

	return 0;
}
