#pragma once


typedef unsigned int (*FN_HOOK)(void);
typedef unsigned int (*FN_UNHOOK)(void);

extern FN_HOOK		CLP_SetHook;
extern FN_UNHOOK	CLP_UnsetHook;

unsigned int CLP_Init();