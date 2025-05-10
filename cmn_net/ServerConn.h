#pragma once


#include <string.h>
#include "curl/curl.h"
#include "json.hpp"
#include <string>
#include <cstring>
#include <fstream> // Include for std::ofstream
#include <iostream>

#include "KC_common.h"

typedef void (*FN_CALLBACK_ADD)(
	int p_nID,
	const char* p_szName,
	const char* p_szDesc,
	const char* p_szSize,
	const char* p_szPwd
	);

typedef void (*FN_CALLBACK_PORT)(
	const char* p_szRemoteName,
	int p_szRemotePort,
	int p_szLocalPort);

typedef void (*FN_CALLBACK_HTTPS)(
	int p_nID,
	const char *p_szTitle,
	int p_nLocalPort,
	const char *p_szRemote,
	int p_nRemotePort,
	BOOL p_bIsHttps,
	BOOL p_bIsActive
	);	

typedef bool(*FN_CALLBACK_DOWN)(
	long long lCur,
	long long lMax
	);

typedef struct tagSetting
{
	int idleTime;
	bool allowCopyin;
}ST_SETTING;


const std::string IDENTIFICATION_KEY = "__kenan_header__";

std::string getMessage();

void setServer(const wchar_t* p_wszServer, int p_nPort);

std::string authenticateUser(const std::string& username, const std::string& password, const std::string& machine);

std::string getValidateMessage(int &speed, int& result, char* p_szHealthRate);
std::string validateAuth(const std::string& userID, const std::string& OldJwt, const std::string& machineID, const std::string& ip);

void getSessionID(char* p_pBuff, int cch);

std::string otpAuth(const std::string& mail, const std::string& pwd, const std::string& machine, const std::string& opt, const std::string& ip, int& p_nUserID);

int downloadFile(const char* p_szUserID, const std::string& token, const std::string& filename, const std::string& destination, const std::string& machine_id,const std::string& file_url, FN_CALLBACK_DOWN p);


ClaKcPolicy* requestDownList(const std::string& token, const std::string& userid);
ClaKcGroup* requestDownConfig(const std::string& token, const std::string& userid, int p_nGroupID, int p_nImageID, const std::string& machineid);
ClaKcVersions* requestDownUpdateVersion(const std::string& token, const std::string& userid, const std::string& machineid);

unsigned int request_set_password(
	const char* lv_szGWP,
	const char* lv_szUserID,
	const char* lv_szMachineID,
	int p_nImageID,
	const char* p_szKey
);
unsigned int request_get_password(
	const char* lv_szGWP,
	const char* lv_szUserID,
	const char* lv_szMachineID,
	int p_nImageID,
	char* p_szKey, int p_cchKey, 
	char* p_szPwd, int p_cchPwd
);

unsigned int requestReportAction(
	const std::string& token, const std::string& machineID, const std::string& userID,
	int p_nVmImageID, const char* p_szAction);
unsigned int requestReportViolation(
	const std::string& token, const std::string& machineID, const std::string& userID,
	int vmimage_id, int violation_id, int script_id, int action1_done, int action2_done, const char* report);
	//const std::string& token, const std::string& userid, int p_nGroupID, int p_nImageID, const std::string& machineid);
unsigned int requestReportAgentScript(
	const std::string& token, const std::string& machineID, const std::string& userID,
	int vmimage_id, int script_id, int status, const char* report);
unsigned int requestUploadUpdate(const std::string& p_serverUrl, const std::string& p_fileUrl);


int uploadPort(const std::string& ip, const std::string& data);

int uploadViolation(const std::string& ip, const std::string& data);

int uploadAgentScript(const std::string& ip, const std::string& data);

int uploadScript(const std::string& ip, const std::string& data);

int uploadIp(const std::string& ip, const std::string& data, char* p_szOS, int cchOS, char* p_szMAC, int cchMAC);

int requestIdle(const std::string& ip, int* p_pnIdle, std::string* scriptResult, int* violationID, int* scriptID, char* agentScripts);

int requestAgentVersion(const std::string& ip, char* agentVersion);

int uploadStatus(const std::string& token, int p_nStatus);

const char* getGlobalIP();

extern float g_downloadProgress;