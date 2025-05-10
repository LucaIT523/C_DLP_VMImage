#pragma once

#include <string>
#include "ClaPathMgr.h"

typedef struct tagKcVmInfo
{
	int m_nID;
	wchar_t m_wszName[128];
	wchar_t m_wszDescription[256];
	wchar_t m_wszOS[64];
	wchar_t m_wszPassword[64];
	wchar_t m_wszCPU[32];
	wchar_t m_wszRAM[32];
	wchar_t m_wszSize[32];
}KC_VM_INFO;

typedef struct tagKcPortInfo
{
	int m_nPortLocal;
	wchar_t m_wszRemote[128];
	int m_nPortRemote;
}KC_PORT_INFO;

typedef struct tagKcHttpsInfo
{
	int m_nID;
	char m_szTitle[30];
	int m_nLocalPort;
	char m_szRemote[64];
	int m_nRemotePort;
	BOOL m_bIsHttps;
	BOOL m_bIsActive;
}KC_HTTPS_INFO;

typedef struct tagKcSetting
{
	BOOL m_bAllowCopyToVM;
	int m_nAllowIdleSecond;
}KC_SETTING;

class ClaKcVmImage {
public:
	ClaKcVmImage() {
		memset(_szID, 0, sizeof(_szID));
		memset(_szTitle, 0, sizeof(_szTitle));
		memset(_szDesc, 0, sizeof(_szDesc));
		memset(_szOS, 0, sizeof(_szOS));
		memset(_szPwd, 0, sizeof(_szPwd));
		memset(_wszPath, 0, sizeof(_wszPath));
		memset(_szReqOS, 0, sizeof(_szReqOS));
		memset(_szReqAntiVirus, 0, sizeof(_szReqAntiVirus));

		memset(_szDropboxAppKey, 0, sizeof(_szDropboxAppKey));
		memset(_szDropboxAppSecret, 0, sizeof(_szDropboxAppSecret));
		memset(_szDropboxAppRefresgToken, 0, sizeof(_szDropboxAppRefresgToken));
		memset(_szDropboxVmPath, 0, sizeof(_szDropboxVmPath));
	};
	~ClaKcVmImage() {};

public:
	int _id;
	char _szID[16];
	char _szTitle[128];
	char _szDesc[256];
	char _szOS[128];
	char _szPwd[64];
	wchar_t _wszPath[260];
	unsigned long long _lSize;

	char _szReqOS[128];
	unsigned long long _nReqMemory;
	unsigned long long _nReqFreeSpace;
	int _nReqCpuCount;
	int _nReqCoreCount;
	char _szReqAntiVirus[128];

	char _szDropboxVmPath[MAX_PATH];
	char _szDropboxAppKey[20]; //igwuoboctjx7qpy;
	char _szDropboxAppSecret[20]; //16zqn0m8v8qs6lo;
	char _szDropboxAppRefresgToken[70]; //pLwmt65hg5oAAAAAAAAAATcimPjaseA2kaT-iw3nZfRyJkfBYYCbVVk6Yxyhcyxg;

	void operator=(ClaKcVmImage& img) {
		_id = img._id;
		strcpy_s(_szID, 16, img._szID);
		strcpy_s(_szTitle, 128, img._szTitle);
		strcpy_s(_szDesc, 256, img._szDesc);
		strcpy_s(_szOS, 128, img._szOS);
		strcpy_s(_szPwd, 64, img._szPwd);
		wcscpy_s(_wszPath, 260, img._wszPath);

		strcpy_s(_szReqOS, 128, img._szReqOS);
		_nReqMemory = img._nReqMemory;
		_nReqFreeSpace = img._nReqFreeSpace;
		_nReqCpuCount = img._nReqCpuCount;
		_nReqCoreCount = img._nReqCoreCount;
		strcpy_s(_szReqAntiVirus, 128, img._szReqAntiVirus);
		_lSize = img._lSize;

		strcpy_s(_szDropboxVmPath, MAX_PATH, img._szDropboxVmPath);
		strcpy_s(_szDropboxAppKey, 20, img._szDropboxAppKey);
		strcpy_s(_szDropboxAppSecret, 20, img._szDropboxAppSecret);
		strcpy_s(_szDropboxAppRefresgToken, 70, img._szDropboxAppRefresgToken);
	}
	CString _findVMX(const wchar_t* p_wszDir) {
		CString strFind; strFind.Format(L"%s\\*", p_wszDir);
		CFileFind finder;
		BOOL bWorking = finder.FindFile(strFind);

		while (bWorking) {
			bWorking = finder.FindNextFile();
			if (finder.IsDots()) continue;
			if (finder.IsDirectory()) {
				CString strRet = _findVMX(finder.GetFilePath());
				if (!strRet.IsEmpty()) return strRet;
			}
			else {
				CString strRet = finder.GetFilePath();
				if (strRet.Right(4).CompareNoCase(L".vmx") == 0) {
					return strRet;
				}
			}
		}

		return L"";
	}
	void findVMX(void) {
		CString dir;
		dir.Format(L"%s\\VMs\\%S.e", ClaPathMgr::GetDP(), _szTitle);
		dir = _findVMX(dir);
//		dir.Replace(L".kvx", L".vmx");
		swprintf_s(_wszPath, 260,  L"%s", dir.GetBuffer());
	};
};

class ClaKcConfig {
public:
	ClaKcConfig() {
		memset(_wszEncPwd, 0, sizeof(_wszEncPwd));
	};
	~ClaKcConfig() {};

public:
	BOOL _bAllowTextToVM;
	BOOL _bAllowTextFromVM;
	BOOL _bAllowFileToVM;
	BOOL _bAllowFileFromVM;
	BOOL _bAllowCaptureScreen;
	BOOL _bCreateOutboundRule;
	wchar_t _wszEncPwd[100];

	void operator=(ClaKcConfig& img) {
		_bAllowTextToVM = img._bAllowTextToVM;
		_bAllowTextFromVM = img._bAllowTextFromVM;
		_bAllowFileToVM = img._bAllowFileToVM;
		_bAllowFileFromVM = img._bAllowFileFromVM;
		_bAllowCaptureScreen = img._bAllowCaptureScreen;
		_bCreateOutboundRule = img._bCreateOutboundRule;
		memset(_wszEncPwd, 0, sizeof(_wszEncPwd));
		wcscpy_s(_wszEncPwd, 100, img._wszEncPwd);
	};

};

class ClaKcPort {

public:
	ClaKcPort() {
		memset(_szTitle, 0, sizeof(_szTitle));
		memset(_szTarget, 0, sizeof(_szTarget));
	};
	~ClaKcPort() {};

public:
	char _szTitle[128];
	int _nSourcePort;
	char _szTarget[128];
	int _nTargetPort;
	BOOL _bIsHttps;
	BOOL _bIsDirect;

	void operator=(ClaKcPort& img) {
		strcpy_s(_szTitle, 128, img._szTitle);
		_nSourcePort = img._nSourcePort;
		strcpy_s(_szTarget, 128, img._szTarget);
		_nTargetPort = img._nTargetPort;
		_bIsHttps = img._bIsHttps;
		_bIsDirect = img._bIsDirect;
	};

};

class ClaKcPolicyScript {

public:
	ClaKcPolicyScript() {
	};

	ClaKcPolicyScript(const ClaKcPolicyScript& other) {
		_nID = other._nID;
		_title = other._title;
		_script = other._script;
		_expected_result = other._expected_result;
		_os = other._os;
	}

	~ClaKcPolicyScript() {};

public:
	int _nID;
	std::string _title;
	std::string _script;
	std::string _expected_result;
	std::string _os;

	void operator=(ClaKcPolicyScript& img) {
		_nID = img._nID;
		_title = img._title;
		_script = img._script;
		_expected_result = img._expected_result;
		_os = img._os;
	};
	BOOL operator !=(ClaKcPolicyScript& img) {
		if (_nID != img._nID) return TRUE;
		if (_title != img._title) return TRUE;
		if (_script != img._script) return TRUE;
		if (_expected_result != img._expected_result) return TRUE;
		if (_os != img._os) return TRUE;
		return FALSE;
	}
};

class ClaKcAgentScriptPlan {

public:
	ClaKcAgentScriptPlan() {};
	~ClaKcAgentScriptPlan() {};

public:
	int _nID;
	int _agent_script_id;
	std::string _agent_script_title;
	std::string _agent_script;
	std::string _os;
	int _execution_type;
	unsigned long long _scheduled_time;
	BOOL _run_at_once;

	void operator=(ClaKcAgentScriptPlan& img) {
		_nID = img._nID;
		_agent_script_id = img._agent_script_id;
		_agent_script_title = img._agent_script_title;
		_agent_script = img._agent_script;
		_os = img._os;
		_execution_type = img._execution_type;
		_scheduled_time = img._scheduled_time;
		_run_at_once = img._run_at_once;
	};
};

class ClaKcVersion {

public:
	ClaKcVersion() {};
	~ClaKcVersion() {};

public:
	int _nID;
	int _action_type;
	std::string _version;
	std::string _release_date;
	std::string _change_log;
	std::string _platform;

	void operator=(ClaKcVersion& img) {
		_nID = img._nID;
		_action_type = img._action_type;
		_version = img._version;
		_release_date = img._release_date;
		_change_log = img._change_log;
		_platform = img._platform;
	};
};
class ClaKcViolation {

public:
	ClaKcViolation() {               
	};
	ClaKcViolation(ClaKcViolation& img) {
		_nID = img._nID;
		_title = img._title;
		_action1 = img._action1;
		_action2 = img._action2;
		_os = img._os;
		_check_at_start = img._check_at_start;
		_check_interval = img._check_interval;
		_message = img._message;
		_show_warning = img._show_warning;

		_policy_scripts.RemoveAll();
		for (int i = 0; i < img._policy_scripts.GetCount(); i++) {
			ClaKcPolicyScript item = img._policy_scripts[i];
			_policy_scripts.Add(item);
		}
	};
	~ClaKcViolation() {
		_policy_scripts.RemoveAll();
	};

public:
	int _nID;
	std::string _title;
	int _action1;
	int _action2;
	std::string _os;
	bool _check_at_start;
	int _check_interval;
	std::string _message;
	bool _show_warning;

	CArray<ClaKcPolicyScript, ClaKcPolicyScript&> _policy_scripts;

	void operator=(ClaKcViolation& img) {
		_nID = img._nID;
		_title = img._title;
		_action1 = img._action1;
		_action2 = img._action2;
		_os = img._os;
		_check_at_start = img._check_at_start;
		_check_interval = img._check_interval;
		_message = img._message;
		_show_warning = img._show_warning;

		_policy_scripts.RemoveAll();
		for (int i = 0; i < img._policy_scripts.GetCount(); i++) {
			ClaKcPolicyScript item = img._policy_scripts[i];
			_policy_scripts.Add(item);
		}
	};

};
class ClaKcVersions {
public:
	ClaKcVersions() { 
		launcher = new ClaKcVersion(); 
		agent = new ClaKcVersion();
	};
	~ClaKcVersions() {
		delete launcher;
		delete agent;
	};
	ClaKcVersion* launcher;
	ClaKcVersion* agent;
};

class ClaKcGroup {
public:
	ClaKcGroup() { 
		memset(_szID, 0, sizeof(_szID)); 
		memset(_szName, 0, sizeof(_szName));
	};
	~ClaKcGroup() {
		while (_lstVmImage.GetCount()) {
			_lstVmImage.RemoveAt(0);
		}
		while (_lstPort.GetCount()) {
			_lstPort.RemoveAt(0);
		}
	};

public:
	int _id;
	char _szID[16];
	char _szName[64];

	ClaKcConfig _config;
	CArray< ClaKcVmImage, ClaKcVmImage&> _lstVmImage;
	CArray< ClaKcPort, ClaKcPort&> _lstPort;
	CArray< ClaKcViolation, ClaKcViolation&> _lstViolation;
	CArray< ClaKcAgentScriptPlan, ClaKcAgentScriptPlan&> _lstAgentScriptPlan;

	void operator=(ClaKcGroup& p) {
		_id = p._id;
		strcpy_s(_szID, 16, p._szID);
		strcpy_s(_szName, 64, p._szName);

		_config = p._config;

		while(_lstVmImage.GetCount() > 0)
			_lstVmImage.RemoveAt(0);
		for (int i = 0; i < p._lstVmImage.GetCount(); i++) {
			ClaKcVmImage image = p._lstVmImage.GetAt(i);
			_lstVmImage.Add(image);
		}

		while(_lstPort.GetCount() > 0)
			_lstPort.RemoveAt(0);
		for (int i = 0; i < p._lstPort.GetCount(); i++) {
			ClaKcPort port = p._lstPort.GetAt(i);
			_lstPort.Add(port);
		}

		_lstViolation.RemoveAll();
		for (int i = 0; i < p._lstViolation.GetCount(); i++) {
			ClaKcViolation violation (p._lstViolation.GetAt(i));
			_lstViolation.Add(violation);
		}

		_lstAgentScriptPlan.RemoveAll();
		for (int i = 0; i < p._lstAgentScriptPlan.GetCount(); i++) {
			ClaKcAgentScriptPlan agentScript = p._lstAgentScriptPlan.GetAt(i);
			_lstAgentScriptPlan.Add(agentScript);
		}
	};
	BOOL operator!=(ClaKcGroup& p) {
		if (_config._bAllowCaptureScreen != p._config._bAllowCaptureScreen) return TRUE;
		if (_config._bAllowFileFromVM != p._config._bAllowFileFromVM) return TRUE;
		if (_config._bAllowFileToVM != p._config._bAllowFileToVM) return TRUE;
		if (_config._bAllowTextFromVM != p._config._bAllowTextFromVM) return TRUE;
		if (_config._bAllowTextToVM != p._config._bAllowTextToVM) return TRUE;
		if (_config._bCreateOutboundRule != p._config._bCreateOutboundRule) return TRUE;

		if (_lstPort.GetCount() != p._lstPort.GetCount()) {
			return TRUE;
		}

		if (comparePort(p) != 0) {
			return TRUE;
		}
		if (compareScriptViolation(p) != 0) {
			return TRUE;
		}
		if (compareScriptPlan(p) != 0) {
			return TRUE;
		}
		return FALSE;
	};
	int comparePort(ClaKcGroup& p) {
		if (_lstPort.GetCount() != p._lstPort.GetCount()) {
			return 1;
		}
		for (int i = 0; i < _lstPort.GetCount(); i++) {
			if (_lstPort[i]._nSourcePort != p._lstPort[i]._nSourcePort) return 1;
			if (_lstPort[i]._nTargetPort != p._lstPort[i]._nTargetPort) return 1;
			if (strcmp(_lstPort[i]._szTarget, p._lstPort[i]._szTarget) != 0) return 1;
		}
		return 0;
	};
	int compareScriptViolation(ClaKcGroup& p) {
		if (_lstViolation.GetCount() != p._lstViolation.GetCount()) {
			return 1;
		}
		for (int i = 0; i < _lstViolation.GetCount(); i++) {
			if (_lstViolation[i]._nID != p._lstViolation[i]._nID) return 1;
			if (_lstViolation[i]._title != p._lstViolation[i]._title) return 1;
			if (_lstViolation[i]._action1 != p._lstViolation[i]._action1) return 1;
			if (_lstViolation[i]._action2 != p._lstViolation[i]._action2) return 1;
			if (_lstViolation[i]._check_at_start != p._lstViolation[i]._check_at_start) return 1;
			if (_lstViolation[i]._check_interval != p._lstViolation[i]._check_interval) return 1;
			if (_lstViolation[i]._message != p._lstViolation[i]._message) return 1;
			if (_lstViolation[i]._os != p._lstViolation[i]._os) return 1;
			if (_lstViolation[i]._show_warning != p._lstViolation[i]._show_warning) return 1;
			for (int j = 0; j < _lstViolation[i]._policy_scripts.GetCount(); j++) {
				if (_lstViolation[i]._policy_scripts[j] != p._lstViolation[i]._policy_scripts[j]) return 1;
			}
		}
		return 0;
	};
	int compareScriptPlan(ClaKcGroup& p) {
		if (_lstAgentScriptPlan.GetCount() != p._lstAgentScriptPlan.GetCount()) {
			return 1;
		}
		for (int i = 0; i < _lstAgentScriptPlan.GetCount(); i++) {
			if (_lstAgentScriptPlan[i]._nID					!= p._lstAgentScriptPlan[i]._nID				) return 1;
			if (_lstAgentScriptPlan[i]._agent_script_id		!= p._lstAgentScriptPlan[i]._agent_script_id	) return 1;
			if (_lstAgentScriptPlan[i]._agent_script_title	!= p._lstAgentScriptPlan[i]._agent_script_title	) return 1;
			if (_lstAgentScriptPlan[i]._agent_script		!= p._lstAgentScriptPlan[i]._agent_script		) return 1;
			if (_lstAgentScriptPlan[i]._os					!= p._lstAgentScriptPlan[i]._os					) return 1;
			if (_lstAgentScriptPlan[i]._execution_type		!= p._lstAgentScriptPlan[i]._execution_type		) return 1;
			if (_lstAgentScriptPlan[i]._scheduled_time		!= p._lstAgentScriptPlan[i]._scheduled_time		) return 1;
			if (_lstAgentScriptPlan[i]._run_at_once			!= p._lstAgentScriptPlan[i]._run_at_once		) return 1;
		}
		return 0;
	};
};

class ClaKcPolicy {
public:
	ClaKcPolicy() { agent_timeout = 0; session_expircy_time = 0; };
	~ClaKcPolicy() {
		while (_lstGroup.GetCount())
			_lstGroup.RemoveAt(0);
	};

public:
	CArray<ClaKcGroup, ClaKcGroup&> _lstGroup;
	int session_expircy_time;
	int agent_timeout;

public:
	void operator=(ClaKcPolicy& p) {
		_lstGroup.RemoveAll();
		for (int i = 0; i < p._lstGroup.GetCount(); i++) {
			_lstGroup.Add(p._lstGroup.GetAt(i));
			session_expircy_time = p.session_expircy_time;
			agent_timeout = p.agent_timeout;
		}
	};
};

typedef BOOL(*KC_CALLBACK_DOWN_VM)(long long p_lCur, long long p_lTotal);
typedef BOOL(*KC_CALLBACK_DOWN_LANUCHER)(long long p_lCur, long long p_lTotal);

int kc_init(const wchar_t* p_wszServer, int p_nPort);
typedef void (*fn_kc_get_uid)(char* p_szID);
extern fn_kc_get_uid kc_get_uid;
typedef DWORD (*fn_kc_get_err)();
extern fn_kc_get_err kc_get_err;
typedef unsigned int (*fn_kc_get_message)(const wchar_t* p_wszMessage, int cch);
extern fn_kc_get_message kc_get_message;
typedef unsigned int (*fn_kc_auth)(const wchar_t* p_wszUser, const wchar_t* p_wszPassword, const wchar_t* p_wszMachine);
extern fn_kc_auth kc_auth;
typedef unsigned int (*fn_kc_get_session)(char* p_szBuff, int cch);
extern fn_kc_get_session kc_get_session;

typedef BOOL(*FN_CALLBACK_VALIDAT)(void* p, int result, const char* p_szMessage, int p_nSpeed, char* p_szHealth);

typedef unsigned int (*fn_kc_otp)(const wchar_t* p_wszMachine, const wchar_t* p_wszOTP, const wchar_t* p_wszIP, FN_CALLBACK_VALIDAT fn, void* param);
extern fn_kc_otp kc_otp;
typedef const char* (*fn_kc_get_status)(int* qr_state);
extern fn_kc_get_status kc_get_status;
typedef KC_VM_INFO* (*fn_kc_down_vmlist)(int& p_nCount);
extern fn_kc_down_vmlist kc_down_vmlist;
typedef int (*fn_kc_down_vmimage)(
	const char* p_szDropboxPath,
	const wchar_t* p_wszDestination,
	BOOL p_bContinue,
	const char* p_szDropboxAppKey,
	const char* p_szDropboxAppSecret,
	const char* p_szDropboxRefreshToken,
	KC_CALLBACK_DOWN_VM p_fnCallback);
extern fn_kc_down_vmimage kc_down_vmimage;
typedef int (*fn_kc_down_vmimage_size)(
	const char* p_szDropboxPath,
	const char* p_szDropboxAppKey,
	const char* p_szDropboxAppSecret,
	const char* p_szDropboxRefreshToken,
	unsigned long long* p_plSize);
extern fn_kc_down_vmimage_size kc_down_vmimage_size;
typedef int (*fn_kc_up_vmimage_status)(
	int p_nVmImageID,
	int p_nStatus
);
extern fn_kc_up_vmimage_status kc_up_vmimage_status;
typedef KC_PORT_INFO* (*fn_kc_down_portlist)(int& p_nCount);
extern fn_kc_down_portlist kc_down_portlist;

typedef int (*fn_kc_up_port)(const char* p_szServer, const char* p_szData);
extern fn_kc_up_port kc_up_port;

typedef int (*fn_kc_up_violation)(const char* p_szServer, const char* p_szData);
extern fn_kc_up_violation kc_up_violation;
typedef int (*fn_kc_up_agent_script)(const char* p_szServer, const char* p_szData);
extern fn_kc_up_agent_script kc_up_agent_script;

typedef int (*fn_kc_get_agent_version)(const char* p_szServer, char* szVersion);
extern fn_kc_get_agent_version kc_get_agent_version;

typedef int (*fn_kc_up_ip)(const char* p_szServer, const char* p_pHostIP, char* p_szOS, int cchOS, char* p_szMAC, int cchMAC);
extern fn_kc_up_ip kc_up_ip;

typedef int (*fn_kc_get_idle)(const char* p_szServer, int* p_pIdleTime, char* p_scriptResult, int* p_violationId, int* p_scriptID, char* agentScripts);
extern fn_kc_get_idle kc_get_idle;

typedef KC_HTTPS_INFO* (*fn_kc_down_httpslist)(int& p_nCount);
extern fn_kc_down_httpslist kc_down_httpslist;

typedef int (*fn_kc_down_setting)(KC_SETTING* p_pstSetting);
extern fn_kc_down_setting kc_down_setting;

typedef ClaKcPolicy* (*fn_kc_down_list)();
extern fn_kc_down_list kc_down_list;

typedef unsigned int (*fn_kc_down_update_file)(int ver, const wchar_t* p_destination);
extern fn_kc_down_update_file kc_down_update_file;

typedef ClaKcGroup* (*fn_kc_down_config)(int p_nGroupID, int p_nImageID);
extern fn_kc_down_config kc_down_config;

typedef ClaKcVersions* (*fn_kc_down_check_update)();
extern fn_kc_down_check_update kc_down_check_update;

typedef unsigned int (*fn_kc_report_action)(int vmimage_id, const char* p_szAction);
extern fn_kc_report_action kc_report_action;

typedef unsigned int (*fn_kc_get_password)(int p_nImageID, char* p_szKey, int p_cchKey, char* p_szPwd, int p_cchPwd);
extern fn_kc_get_password kc_get_password;
typedef unsigned int (*fn_kc_set_password)(int p_nImageID, const char* p_szKey);
extern fn_kc_set_password kc_set_password;

typedef unsigned int (*fn_kc_report_violation)(int vmimage_id, int violation_id, int script_id, int action1_done, int action2_done, const char* report);
extern fn_kc_report_violation kc_report_violation;

typedef unsigned int (*fn_kc_report_agent_script)(int vmimage_id, int script_id, int status, const char* report);
extern fn_kc_report_agent_script kc_report_agent_script;

typedef unsigned int (*fn_kc_upload_update)(const char* p_serverUrl, const wchar_t* p_fileUrl);
extern fn_kc_upload_update kc_upload_update;

typedef const char* (*fn_kc_get_global_ip)();
extern fn_kc_get_global_ip kc_get_global_ip;

typedef void (*fn_kc_release)();
extern fn_kc_release kc_release;

typedef void (*fn_kc_uninit)();
extern fn_kc_uninit kc_uninit;

