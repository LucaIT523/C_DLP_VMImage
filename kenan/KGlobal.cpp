#include "pch.h"
#include "KGlobal.h"

#include "kncrypto.h"
#include "ClaRegMgr.h"

#include "ClaFileMap.h"
#include "ini.h"
#include "ClaPathMgr.h"

ClaFileMap lv_filemapPol;

#ifdef _LAUNCHER_
#include "ClaNetAdapter.h"
#include "SHA256.h"


CString KGlobal::GetMachineID(void)
{
	CString strMAC = ClaNetAdapter::GetMAC();

	char szMAC[32]; memset(szMAC, 0, sizeof(szMAC));
	sprintf_s(szMAC, 32, "%S", strMAC);

	SHA256 sha;
	sha.update((const uint8_t*)szMAC, 17);
	std::array<uint8_t, 32> kk = sha.digest();
	
	CString ret; 
	ret.Format(L"%02x%02x-%02x%02x-%02x%02x",
		kk[6], kk[5], kk[8], kk[7], kk[12], kk[30]
	);
	return ret;
}

#endif

typedef struct tagConfig {
	tagConfig() {
		memset(this, 0, sizeof(pBuff));
	};

	union {
		struct {
			DWORD _mark;
			BOOL _bCopyTextToVM;
			BOOL _bCopyTextFromVM;
			BOOL _bCopyFileToVM;
			BOOL _bCopyFileFromVM;
			BOOL _bAllowCaptureScreen;
			BOOL _bCreateOutboundRule;
			wchar_t _wszEncPwd[100];
		};
		unsigned char pBuff[256];
	};

}ST_KC_CONFIG;

void KGlobal::readConfig(ClaKcConfig* p_pConfig)
{
	p_pConfig->_bAllowTextToVM		= FALSE;
	p_pConfig->_bAllowTextFromVM	= FALSE;
	p_pConfig->_bAllowFileToVM		= FALSE;
	p_pConfig->_bAllowFileFromVM	= FALSE;
	p_pConfig->_bAllowCaptureScreen = FALSE;
	p_pConfig->_bCreateOutboundRule = TRUE;

	ST_KC_CONFIG conf;
	DWORD dwSize = sizeof(conf);
	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	reg.readBinary(L"Software\\kenan", L"Policy", (unsigned char*)&conf, &dwSize);
	kn_decrypt_with_pwd((unsigned char*)&conf, sizeof(conf), L"KENAN-REG-CONFIG-0819");

	if (conf._mark != 'kn24') {
		return;
	}

	p_pConfig->_bAllowTextToVM		= conf._bCopyTextToVM;
	p_pConfig->_bAllowTextFromVM	= conf._bCopyTextFromVM;
	p_pConfig->_bAllowFileToVM		= conf._bCopyFileToVM;
	p_pConfig->_bAllowFileFromVM	= conf._bCopyFileFromVM;
	p_pConfig->_bAllowCaptureScreen = conf._bAllowCaptureScreen;
	p_pConfig->_bCreateOutboundRule = conf._bCreateOutboundRule;
	memset(p_pConfig->_wszEncPwd, 0, sizeof(p_pConfig->_wszEncPwd));
	wcscpy_s(p_pConfig->_wszEncPwd, 100, conf._wszEncPwd);

}
void KGlobal::writeConfig(ClaKcConfig* p_pConfig, BOOL p_bQr /*= TRUE*/)
{
	ST_KC_CONFIG conf;
	
	conf._bCopyTextToVM			= p_pConfig->_bAllowTextToVM		;
	conf._bCopyTextFromVM		= p_pConfig->_bAllowTextFromVM		;
	conf._bCopyFileToVM			= p_pConfig->_bAllowFileToVM		;
	conf._bCopyFileFromVM		= p_pConfig->_bAllowFileFromVM		;
	conf._bAllowCaptureScreen	= p_pConfig->_bAllowCaptureScreen	;
	conf._bCreateOutboundRule	= p_pConfig->_bCreateOutboundRule	;
	wcscpy_s(conf._wszEncPwd, 100, p_pConfig->_wszEncPwd);

	if (!p_bQr) {
		conf._bAllowCaptureScreen = TRUE;
	}

	CString strOutput; strOutput.Format(L"SfT - write config = [%d, %d, %d, %d, %d, %d] -- %d",
		conf._bCopyTextToVM, conf._bCopyTextFromVM,
		conf._bCopyFileToVM, conf._bCopyFileFromVM,
		conf._bAllowCaptureScreen, conf._bCreateOutboundRule, 
		GetCurrentProcessId()
		);
	OutputDebugString(strOutput);

	conf._mark = 'kn24';

	kn_encrypt_with_pwd((unsigned char*)&conf, sizeof(conf), L"KENAN-REG-CONFIG-0819");
	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	reg.createKey(L"Software\\kenan");
	reg.writeBinary(L"Software\\kenan", L"Policy", (unsigned char*) & conf, sizeof(conf));
}

void KGlobal::writeClipState(BOOL p_bStatus)
{
	void* pPol = NULL;
	lv_filemapPol.openMap(L"Global\\kpol", &pPol);
	if (pPol != NULL) ((BOOL*)pPol)[0] = p_bStatus;

	//ClaRegMgr reg(HKEY_CURRENT_USER);
	//reg.createKey(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths");
	//reg.writeInt(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths", L"auto", p_bStatus);
}

void KGlobal::readClipState(BOOL& p_bStatus)
{
	void* pPol;
	if (lv_filemapPol.openMap(L"Global\\kpol", &pPol) != 0) {
		p_bStatus = FALSE; return;
	}

	if (pPol != NULL) p_bStatus = ((BOOL*)pPol)[0];
	else p_bStatus = FALSE;
}

void KGlobal::readServerIP(wchar_t* p_wszServer, int cch, wchar_t* p_wszPort, int cchPort, int *p_pPeriodPolicyUpdate)
{
	char szIniPath[MAX_PATH]; memset(szIniPath, 0, sizeof(szIniPath));
	sprintf_s(szIniPath, "%S\\config_client.ini", ClaPathMgr::GetDP());

	mINI::INIFile file(szIniPath);
	mINI::INIStructure ini;

	file.read(ini);

	swprintf_s(p_wszServer, cch, L"%S", ini["settings"]["gw_ip"].c_str());
	swprintf_s(p_wszPort, cchPort, L"%S", ini["settings"]["gw_port"].c_str());
	if (p_pPeriodPolicyUpdate) {
		*p_pPeriodPolicyUpdate = atoi(ini["settings"]["period_update"].c_str());
		*p_pPeriodPolicyUpdate = *p_pPeriodPolicyUpdate ? *p_pPeriodPolicyUpdate : 30;
	}
}
void KGlobal::writeServerIP(const wchar_t* p_wszServer, const wchar_t* p_wszPort)
{
	char szIniPath[MAX_PATH]; memset(szIniPath, 0, sizeof(szIniPath));
	sprintf_s(szIniPath, "%S\\config_client.ini", ClaPathMgr::GetDP());

	mINI::INIFile file(szIniPath);
	mINI::INIStructure ini;

	file.read(ini);

	char szServer[200]; memset(szServer, 0, sizeof(szServer));
	char szPort[200]; memset(szPort, 0, sizeof(szPort));

	sprintf_s(szServer, 200, "%S", p_wszServer);
	sprintf_s(szPort, 200, "%S", p_wszPort);

	ini["settings"]["gw_ip"] = szServer;
	ini["settings"]["gw_port"] = szPort;

	file.generate(ini);
}
