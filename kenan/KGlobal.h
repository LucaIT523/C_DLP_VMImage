#pragma once

#include "KC_common.h"

class KGlobal
{
public:
	KGlobal() {};
	~KGlobal() {};

public:
	static void readConfig(ClaKcConfig* p_pConfig);
	static void writeConfig(ClaKcConfig* p_pConfig, BOOL p_bQr = TRUE);

	static void writeClipState(BOOL p_bStatus);
	static void readClipState(BOOL &p_bStatus);

	static void readServerIP(wchar_t* p_wszServer, int cch, wchar_t* p_wszPort, int cchPort, int* p_pPeriodPolicyUpdate);
	static void writeServerIP(const wchar_t* p_wszServer, const wchar_t* p_wszPort);

public:
	void setVMHandle(HANDLE p_hProcess) {
		_hProcVM = p_hProcess;
	};

#ifdef _LAUNCHER_
	static CString GetMachineID(void);
#endif

protected:
	HANDLE _hProcVM;
};

