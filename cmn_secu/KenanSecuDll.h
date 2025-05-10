#pragma once

typedef unsigned int (*FN_SECU_CREATE_PROCESS_WITH_DLL)(
	const wchar_t* p_wszProcPath,
	const wchar_t* p_wszCmd,
	const wchar_t* p_wszWorkDir,
	const wchar_t* p_wszDllPath,
	bool p_bShow,
	void* p_phProcessHandle
	);

extern FN_SECU_CREATE_PROCESS_WITH_DLL SECU_CreateProcessWithDll;

typedef unsigned int (*FN_SECU_FINISH)(void);
extern FN_SECU_FINISH SECU_Finish;

typedef unsigned int (*FN_SECU_HOOK)(void);
typedef unsigned int (*FN_SECU_UNHOOK)(void);

extern FN_SECU_HOOK		SECU_SetHook;
extern FN_SECU_UNHOOK	SECU_UnsetHook;

unsigned int SECU_Init();