#pragma once

typedef unsigned int (*FN_VD_CREATE)(const wchar_t* p_wszPath, const wchar_t* p_wszPwd, int p_nSize);
typedef unsigned int (*FN_VD_MOUNT_INIT)();
typedef unsigned int (*FN_VD_MOUNT)(const wchar_t* p_wszPath, const wchar_t* p_wszPwd, wchar_t& p_zDrv);
typedef unsigned int (*FN_VD_WAIT)();

extern FN_VD_CREATE		VD_Create;
extern FN_VD_MOUNT_INIT	VD_MountInit;
extern FN_VD_MOUNT		VD_Mount;
extern FN_VD_WAIT		VD_Wait;

unsigned int VD_Init();
