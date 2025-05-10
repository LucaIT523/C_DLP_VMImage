// KenanConn.cpp : Defines the initialization routines for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "KenanConn.h"
#include "KGlobal.h"

#include <string>
#include "curl/curl.h"
#include "json.hpp"
#include <cstring>
#include <fstream> // Include for std::ofstream
#include <iostream>
#include "KC_common.h"
#include "SHA256.h"
#include "ClaOsInfo.h"
#include "base64enc.h"

#define CURL_STATICLIB 1
#pragma comment(lib, "libcurl.dll.a")

//#define     TEST_DIS_CLIENT_SSL_REQ

using namespace std;
using json = nlohmann::json;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CKenanConnApp, CWinApp)
END_MESSAGE_MAP()

// CKenanConnApp construction
CKenanConnApp::CKenanConnApp()
{}

// The one and only CKenanConnApp object
CKenanConnApp theApp;

// CKenanConnApp initialization
BOOL CKenanConnApp::InitInstance()
{
	CWinApp::InitInstance();
	return TRUE;
}


BOOL lv_bInited = FALSE;

/// 
/// Global Variables.
/// 

//.	this is for client connection
char lv_szServerURL[MAX_PATH];
char lv_szJWT[MAX_PATH];
HANDLE lv_hThreadShake;
DWORD lv_dwLastError;
std::string lv_sLastMessage;
int lv_nUserID;
char lv_szUserID[20];
char lv_szUser[MAX_PATH];
char lv_szPwd[MAX_PATH];
char lv_szMachineID[MAX_PATH];
char lv_szSession[MAX_PATH];
char lv_szIP[64];
char lv_szStatus[30];
int lv_nSpeed;
char lv_szHealth[64];
typedef BOOL(*FN_CALLBACK_VALIDAT)(void* p, int result, const char* p_szMessage, int p_nSpeed, char* p_szHealth);
FN_CALLBACK_VALIDAT lv_fnCallbackValidate;
void* lv_paramCallbackValidate;
char lv_szOS[128];
ClaKcVersions lv_kcVersions;
ClaKcPolicy lv_kcPolicy;
ClaKcGroup lv_kcGroup;

//.	this is for agent connection
int lv_qr_state = -1;


/// <summary>
/// START - Base Functions for CURL 
/// </summary>
typedef BOOL (*FN_CALLBACK_PROGRESS)(unsigned long long lTotal, unsigned long long lNow, void* p_pUserData);
typedef void (*FN_CALLBACK_JSON)(void* p_pJSON, void* p_pUserData);
typedef struct tagCurlUserData {
	FN_CALLBACK_JSON		m_pCallbackJson;
	FN_CALLBACK_PROGRESS	m_pCallbackProgress;
	void*					m_pUserDataJson;
	void*					m_pUserDataProgress;
	FILE*					m_pFileDownload;
	void*					m_pBufferTemp;
	int						m_cchBufferTemp;
}ST_CURL_USERDATA;
size_t WriteDataCallback(void* contents, size_t size, size_t nmemb, ST_CURL_USERDATA* userData) {

	size_t totalSize = size * nmemb;
	if (userData == NULL) return totalSize;

	if (userData->m_pFileDownload) {
		fwrite(contents, 1, totalSize, userData->m_pFileDownload);
	}
	else {
		int nOldPos = userData->m_cchBufferTemp;
		userData->m_cchBufferTemp += totalSize;
		userData->m_pBufferTemp = realloc(userData->m_pBufferTemp, userData->m_cchBufferTemp + 1);
		memcpy(&(((char*)userData->m_pBufferTemp)[nOldPos]), contents, totalSize);
		((char*)userData->m_pBufferTemp)[userData->m_cchBufferTemp] = 0;
	}
	return totalSize;
}
static int ProgressCallback(ST_CURL_USERDATA* userData, curl_off_t dltotal, curl_off_t dlnow) {
	if (userData && userData->m_pCallbackProgress) {
		if (userData->m_pCallbackProgress(dltotal, dlnow, userData->m_pUserDataProgress) == FALSE) {
			return -1;
		}
	}
	return 0; // Return 0 to continue downloading
}
int _request_with_serverurl(
	const char* p_szServerURL, const char* p_szURL, const char* p_szBearer = NULL,
	const char* p_szData = NULL, const wchar_t* p_wszUpload = NULL,
	FN_CALLBACK_JSON p_pCallbackJson = NULL,
	FN_CALLBACK_PROGRESS p_pCallbackProgress = NULL,
	void* p_pUserDataJson = NULL,
	void* p_pUserDataProgress = NULL,
	const wchar_t* p_wszDownload = NULL,
	int max_wait = 0, BOOL p_bContinue = FALSE)
{
	int			nRet = 0;
	CURL		*curl = NULL;
	CURLcode	res;
	char		szUrl[MAX_PATH]; memset(szUrl, 0, sizeof(szUrl));
	unsigned char* pUploadBuff = NULL;
	FILE		*pFileUpload = NULL;
	long long	lUploadFileSize = 0;
	long long	lRead = 0;
	ST_CURL_USERDATA stCurlUserData; memset(&stCurlUserData, 0, sizeof(stCurlUserData));
	stCurlUserData.m_pCallbackJson = p_pCallbackJson;
	stCurlUserData.m_pCallbackProgress = p_pCallbackProgress;
	stCurlUserData.m_pUserDataJson = p_pUserDataJson;
	stCurlUserData.m_pUserDataProgress = p_pUserDataProgress;

	lv_dwLastError = 0;

	sprintf_s(szUrl, "%s%s", p_szServerURL, p_szURL);

	curl = curl_easy_init();
	if (curl == NULL) {
		return -1;
	}

	curl_easy_setopt(curl, CURLOPT_URL, szUrl);
	curl_easy_setopt(curl, CURLOPT_POST, 1L);

	if (max_wait)
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, max_wait);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, p_szData);

	struct curl_slist* headers = NULL;
	if (p_szBearer != NULL && p_szBearer[0] != 0) {
		std::string bearerToken = "Authorization: jwt "; bearerToken += p_szBearer;
		headers = curl_slist_append(headers, (bearerToken.c_str()));
	}
	
	if (p_wszUpload != NULL && p_wszUpload[0] != 0) {
		headers = curl_slist_append(headers, "Content-Type: application/gzip");
	}
	else if (p_szData[0] == '{' || p_szData[0] == '[') {
		headers = curl_slist_append(headers, "Content-Type: application/json");
	}
	else {
		headers = curl_slist_append(headers, "Content-Type: text/plain");
	}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	if (p_wszUpload != NULL && p_wszUpload[0] != 0 && GetFileAttributes(p_wszUpload) != INVALID_FILE_ATTRIBUTES)
	{
		long long lFileSize = ClaPathMgr::GetFileSize(p_wszUpload);
		lUploadFileSize = lFileSize;
		if (lUploadFileSize == -1) {
			nRet = -2;
			goto L_EXIT;
		}
		_wfopen_s(&pFileUpload, p_wszUpload, L"rb");
		if (pFileUpload == NULL) {
			nRet = -2;
			goto L_EXIT;
		}

		pUploadBuff = (unsigned char*)malloc((size_t)lUploadFileSize);
		lRead = 0;
		while (lRead < lUploadFileSize) {
			lRead += fread(&pUploadBuff[lRead], 1, (size_t)lUploadFileSize, pFileUpload);
			if (lRead <= 0) {
				nRet = -2;
				goto L_EXIT;
			}
		}
		fclose(pFileUpload); pFileUpload = NULL;

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pUploadBuff);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, lUploadFileSize);
	}
	else {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, p_szData);
	}

	if (p_wszDownload != NULL && p_wszDownload[0] != 0) {
		//.	write response to file
		if (!p_bContinue)
			_wfopen_s(&stCurlUserData.m_pFileDownload, p_wszDownload, L"wb");
		else
			_wfopen_s(&stCurlUserData.m_pFileDownload, p_wszDownload, L"ab");
	}
	else {
		stCurlUserData.m_pBufferTemp = malloc(1);
	}

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteDataCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&stCurlUserData);

#ifdef TEST_DIS_CLIENT_SSL_REQ
	//. Disable host verification
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

	if (p_pCallbackProgress) {
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L); // Enable progress meter
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, ProgressCallback); // Set progress callback
		curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &stCurlUserData); // Set user data for the callback
	}

	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		nRet = -3;
		goto L_EXIT;
	}

	if (p_wszDownload == NULL || p_wszDownload[0] == 0) {
		if (stCurlUserData.m_cchBufferTemp <= 1) {
			nRet = -4;
			goto L_EXIT;
		}
		try {
			json rsp = json::parse((char*)stCurlUserData.m_pBufferTemp);
			if (rsp["status"] != 200) {
				std::string message = rsp["message"];
				lv_sLastMessage = message;
				nRet = -6;
				goto L_EXIT;
			}

			if (p_pCallbackJson)
				p_pCallbackJson(&rsp, p_pUserDataJson);
		}
		catch (std::exception e) {
			lv_dwLastError = ERROR_SERVER_DISABLED;
			nRet = -5;
			goto L_EXIT;
		}
	}
	nRet = 0;

L_EXIT:
	if (pFileUpload != NULL) {
		fclose(pFileUpload);
	}
	if (pUploadBuff) {
		free(pUploadBuff);
	}
	if (headers != NULL) {
		curl_slist_free_all(headers);
	}
	if (stCurlUserData.m_pBufferTemp)
		free(stCurlUserData.m_pBufferTemp);
	if (stCurlUserData.m_pFileDownload)
		fclose(stCurlUserData.m_pFileDownload);
	curl_easy_cleanup(curl);

	return nRet;
}

int _request_for_dropbox_info(
	const char* p_szURL,
	const char* p_szBearer = NULL,
	const char* p_szData = NULL,
	BOOL p_bFormatedData = FALSE,
	FN_CALLBACK_JSON p_pCallbackJson = NULL,
	void* p_pUserDataJson = NULL
)
{
	int			nRet = 0;
	CURL* curl = NULL;
	CURLcode	res;
	char		szUrl[MAX_PATH]; memset(szUrl, 0, sizeof(szUrl));
	ST_CURL_USERDATA stCurlUserData; memset(&stCurlUserData, 0, sizeof(stCurlUserData));
	stCurlUserData.m_pCallbackJson = p_pCallbackJson;
	stCurlUserData.m_pCallbackProgress = NULL;
	stCurlUserData.m_pUserDataJson = p_pUserDataJson;
	stCurlUserData.m_pUserDataProgress = NULL;

	lv_dwLastError = 0;

	sprintf_s(szUrl, "https://api.dropbox.com%s", p_szURL);

	curl = curl_easy_init();
	if (curl == NULL) {
		return -1;
	}

	curl_easy_setopt(curl, CURLOPT_URL, szUrl);
	curl_easy_setopt(curl, CURLOPT_POST, 1L);

	struct curl_slist* headers = NULL;
	if (p_szBearer != NULL && p_szBearer[0] != 0) {
		std::string bearerToken = "Authorization: Bearer "; bearerToken += p_szBearer;
		headers = curl_slist_append(headers, (bearerToken.c_str()));
	}

	if (p_bFormatedData) {
		headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
	}
	else {
		headers = curl_slist_append(headers, "Content-Type: application/json");
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, p_szData);


	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteDataCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&stCurlUserData);

	//. Disable host verification
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		nRet = -3;
		goto L_EXIT;
	}

	if (stCurlUserData.m_cchBufferTemp <= 1) {
		nRet = -4;
		goto L_EXIT;
	}
	try {
		json rsp = json::parse((char*)stCurlUserData.m_pBufferTemp);
		if (p_pCallbackJson)
			p_pCallbackJson(&rsp, p_pUserDataJson);
	}
	catch (std::exception e) {
		lv_dwLastError = ERROR_SERVER_DISABLED;
		nRet = -5;
		goto L_EXIT;
	}
	nRet = 0;

L_EXIT:
	if (headers != NULL) {
		curl_slist_free_all(headers);
	}
	if (stCurlUserData.m_pBufferTemp)
		free(stCurlUserData.m_pBufferTemp);

	curl_easy_cleanup(curl);

	return nRet;
}

int _request_for_dropbox_download(
	const char* p_szBearer,
	const char* p_szPathOnDropbox,
	unsigned long long p_lStartOffset,
	const wchar_t* p_wszDownload,
	FN_CALLBACK_PROGRESS p_pCallbackProgress = NULL,
	void* p_pUserDataProgress = NULL
)
{
	int			nRet = 0;
	CURL* curl = NULL;
	CURLcode	res;
	char		szUrl[MAX_PATH]; memset(szUrl, 0, sizeof(szUrl));
	char		szPath[MAX_PATH+64]; memset(szPath, 0, sizeof(szPath));
	char		szRange[64];

	ST_CURL_USERDATA stCurlUserData; memset(&stCurlUserData, 0, sizeof(stCurlUserData));
	stCurlUserData.m_pCallbackJson = NULL;
	stCurlUserData.m_pCallbackProgress = p_pCallbackProgress;
	stCurlUserData.m_pUserDataJson = NULL;
	stCurlUserData.m_pUserDataProgress = p_pUserDataProgress;

	lv_dwLastError = 0;

	strcpy_s(szUrl, "https://content.dropboxapi.com/2/files/download");

	curl = curl_easy_init();
	if (curl == NULL) {
		return -1;
	}

	curl_easy_setopt(curl, CURLOPT_URL, szUrl);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

	struct curl_slist* headers = NULL;
	if (p_szBearer != NULL && p_szBearer[0] != 0) {
		std::string bearerToken = "Authorization: Bearer "; bearerToken += p_szBearer;
		headers = curl_slist_append(headers, (bearerToken.c_str()));
	}

	sprintf_s(szPath, MAX_PATH+64, "Dropbox-API-Arg: {\"path\":\"%s\"}", p_szPathOnDropbox);
	headers = curl_slist_append(headers, szPath);

	if (p_lStartOffset > 0) {
		sprintf_s(szRange, 64, "Range: bytes=%lld-", p_lStartOffset);
		headers = curl_slist_append(headers, szRange);
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	//.	write response to file
	if (p_lStartOffset <= 0)
		_wfopen_s(&stCurlUserData.m_pFileDownload, p_wszDownload, L"wb");
	else
		_wfopen_s(&stCurlUserData.m_pFileDownload, p_wszDownload, L"ab");

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteDataCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&stCurlUserData);

	//. Disable host verification
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	if (p_pCallbackProgress) {
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L); // Enable progress meter
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, ProgressCallback); // Set progress callback
		curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &stCurlUserData); // Set user data for the callback
	}

	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		nRet = -3;
		goto L_EXIT;
	}

	nRet = 0;

L_EXIT:
	if (headers != NULL) {
		curl_slist_free_all(headers);
	}
	if (stCurlUserData.m_pBufferTemp)
		free(stCurlUserData.m_pBufferTemp);
	if (stCurlUserData.m_pFileDownload)
		fclose(stCurlUserData.m_pFileDownload);
	curl_easy_cleanup(curl);

	return nRet;
}

typedef struct tagThreadInfoRequest {
	char szServerURL[MAX_PATH];
	char szURL[MAX_PATH];
	char szBearer[MAX_PATH];
	char* szData;
	wchar_t wszUpload[MAX_PATH];
	FN_CALLBACK_JSON pCallbackJson;
	FN_CALLBACK_PROGRESS pCallbackProgress;
	void* pUserDataJson;
	void* pUserDataProgress;
	wchar_t wszDownload[MAX_PATH];
	int max_wait;
	int bContinue;
}ST_THREAD_REQUEST;
static UINT tf_request_with_serverurl_thread(void* p)
{
	ST_THREAD_REQUEST* pParam = (ST_THREAD_REQUEST*)p;
	int nRet = _request_with_serverurl(
		pParam->szServerURL,
		pParam->szURL,
		pParam->szBearer,
		pParam->szData,
		pParam->wszUpload,
		pParam->pCallbackJson,
		pParam->pCallbackProgress,
		pParam->pUserDataJson,
		pParam->pUserDataProgress,
		pParam->wszDownload,
		pParam->max_wait,
		pParam->bContinue
		);
	free(pParam);
	return nRet;
}
HANDLE _request_with_serverurl_thread(
	const char* p_szServerURL, const char* p_szURL, const char* p_szBearer = NULL,
	const char* p_szData = NULL, const wchar_t* p_wszUpload = NULL,
	FN_CALLBACK_JSON p_pCallbackJson = NULL,
	FN_CALLBACK_PROGRESS p_pCallbackProgress = NULL,
	void* p_pUserDataJson = NULL,
	void* p_pUserDataProgress = NULL,
	const wchar_t* p_wszDownload = NULL,
	int max_wait = 0, BOOL p_bContinue = FALSE)
{
	ST_THREAD_REQUEST* pRequest = (ST_THREAD_REQUEST*)malloc(sizeof(ST_THREAD_REQUEST));
	memset(pRequest, 0, sizeof(ST_THREAD_REQUEST));
	strcpy_s(pRequest->szServerURL, MAX_PATH, p_szServerURL);
	strcpy_s(pRequest->szURL, MAX_PATH, p_szURL);
	if (p_szBearer) strcpy_s(pRequest->szBearer, MAX_PATH, p_szBearer);
	pRequest->szData = (char*)p_szData;
	if (p_wszUpload) wcscpy_s(pRequest->wszUpload, MAX_PATH, p_wszUpload);
	pRequest->pCallbackJson = p_pCallbackJson;
	pRequest->pCallbackProgress = p_pCallbackProgress;
	pRequest->pUserDataJson = p_pUserDataJson;
	pRequest->pUserDataProgress = p_pUserDataProgress;
	if (p_wszDownload) wcscpy_s(pRequest->wszDownload, MAX_PATH, p_wszDownload);
	pRequest->max_wait = max_wait;
	pRequest->bContinue = p_bContinue;

	DWORD dwTID;
	return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)tf_request_with_serverurl_thread, pRequest, 0, &dwTID);
}
/// <summary>
/// END - Base Functions for CURL
/// </summary>

/// <summary>
/// KC_INIT is function for initialization of this module.
/// </summary>
/// <param name="p_wszServer"></param>
/// <param name="p_nPort"></param>
/// <returns>0 if success, otherwise return non-zero</returns>
extern "C" __declspec(dllexport) unsigned int KC_INIT(const wchar_t* p_wszServer, int p_nPort) {
	memset(lv_szServerURL, 0, sizeof(lv_szServerURL));
#ifdef TEST_DIS_CLIENT_SSL_REQ
	sprintf_s(lv_szServerURL, 200, "https://%S:%d", p_wszServer, p_nPort);
#else
	sprintf_s(lv_szServerURL, 200, "http://%S:%d", p_wszServer, p_nPort);
#endif

	if (lv_bInited == TRUE) return 0;
	lv_hThreadShake = NULL;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	memset(lv_szOS, 0, sizeof(lv_szOS));
	strcpy_s(lv_szOS, 128, ClaOSInfo::getOS().c_str());
	return 0;
}

/// <summary>
/// Uninitialize this module.
/// </summary>
extern "C" __declspec(dllexport) void KC_UNINIT() {

	memset(lv_szJWT, 0, sizeof(lv_szJWT));
	if (lv_hThreadShake != NULL) {
		TerminateThread(lv_hThreadShake, 0);
	}

	return;
}

extern "C" __declspec(dllexport) DWORD KC_GET_ERR() {
	return lv_dwLastError;
}
extern "C" __declspec(dllexport) void KC_GET_UID(char* p_szID) {
	strcpy_s(p_szID, 5, lv_szUserID);
}
extern "C" __declspec(dllexport) unsigned int KC_GET_MESSAGE(wchar_t* p_wszMessage, int cch)
{
	swprintf_s(p_wszMessage, cch, L"%S", lv_sLastMessage.c_str());
	return 0;
}
extern "C" __declspec(dllexport) void KC_GET_SESSION(char* p_szBuff, int cch)
{
	strcpy_s(p_szBuff, cch, lv_szSession);
}
extern "C" __declspec(dllexport) const char* KC_GET_STATUS(int* p_pnQrState) {
	if (p_pnQrState) *p_pnQrState = lv_qr_state;
	return lv_szStatus;
}

/// <summary>
/// Authenticate user with param - for client
/// </summary>
/// <param name="p_wszUser">const wchar_t* : Email address for user</param>
/// <param name="p_wszPassword">const wchar_t* : password </param>
/// <param name="p_wszMachine">const wchar_t* : machine_id string as XXXX-XXXX-XXXX</param>
/// <returns>return 0 if success, otherwise non-zero</returns>
extern "C" __declspec(dllexport) unsigned int KC_AUTH(const wchar_t* p_wszUser, const wchar_t* p_wszPassword, const wchar_t* p_wszMachine) {
	char szUser[200]; memset(szUser, 0, sizeof(szUser));
	sprintf_s(szUser, 200, "%S", p_wszUser);
	char szPassword[200]; memset(szPassword, 0, sizeof(szPassword));
	sprintf_s(szPassword, 200, "%S", p_wszPassword);
	char szMachine[200]; memset(szMachine, 0, sizeof(szMachine));
	sprintf_s(szMachine, 200, "%S", p_wszMachine);

	SHA256 sha;
	sha.update(szPassword);
	std::string pwd_hash = sha.toString(sha.digest());

	json jsonSend;
	jsonSend["__from_launcher__"] = 1;
	jsonSend["email"] = szUser;
	jsonSend["password"] = pwd_hash.c_str();
	jsonSend["machine_id"] = szMachine;
	jsonSend["os"] = lv_szOS;
	std::string sSend = jsonSend.dump();

	HANDLE hThread = _request_with_serverurl_thread(
		lv_szServerURL, "/api/auth/login", NULL, sSend.c_str(), NULL,
		NULL, NULL, NULL, NULL, NULL, 5);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);
	if (ret != 0) {
		return ret;
	}

	memset(lv_szUser, 0, sizeof(lv_szUser));
	memset(lv_szPwd, 0, sizeof(lv_szPwd));
	memset(lv_szMachineID, 0, sizeof(lv_szMachineID));
	strcpy_s(lv_szUser, MAX_PATH, szUser);
	strcpy_s(lv_szPwd, MAX_PATH, pwd_hash.c_str());
	strcpy_s(lv_szMachineID, MAX_PATH, szMachine);

	return ret;
}

typedef struct tagUserDataValidate {
	int m_nValidate;
	char m_szToken[MAX_PATH];
	char m_szMessage[MAX_PATH];
	int m_nSpeed;
	char m_szHealth[64];

	int m_nResult;
}ST_USERDATA_VALIDATE;
void CALLBACK_VALIDATE(void* p_pJSON, void* p_pUserData) {
	ST_USERDATA_VALIDATE* pData = (ST_USERDATA_VALIDATE*)p_pUserData;
	json* pJson = (json*)p_pJSON;

	try {
		json result = (*pJson)["data"];

		if (!result.contains("is_valid")) return;

		if (result["is_valid"] == 1) {
			std::string jwt = result.contains("token") ? result["token"] : "";
			strcpy_s(pData->m_szToken, 300, jwt.c_str());
			pData->m_nValidate = 1;
		}
		else {
			pData->m_nValidate = 0;
		}

		if (result.contains("message")) {
			std::string sMessage = result["message"];
			strcpy_s(pData->m_szMessage, MAX_PATH, sMessage.c_str());
		}

		if (result.contains("speed")) {
			pData->m_nSpeed = result["speed"];
		}

		if (result.contains("health_rate")) {
			float health_rate = result["health_rate"];
			sprintf_s(pData->m_szHealth, 64, "%.2f", health_rate);
		}
	} catch (std::exception e) {
		pData->m_nResult = -1;
	}
	return;
}
/// <summary>
/// Thread function for validate every 20s - for client.
/// validate will return new jwt, speed, health rate
/// </summary>
/// <param name=""></param>
/// <returns>return 0 if success, otherwise non-zero</returns>
UINT TF_SHAKE(void*) {
	strcpy_s(lv_szStatus, 30, "connected");

	ST_USERDATA_VALIDATE stValidate;

	while (TRUE) {
		Sleep(10000);

		json jsonSend;
		jsonSend["__from_launcher__"] = 1;
		jsonSend["user_id"] = lv_nUserID;
		jsonSend["ip"] = lv_szIP;
		jsonSend["os"] = lv_szOS;
		jsonSend["session_id"] = lv_szSession;
		jsonSend["machine_id"] = lv_szMachineID;
		std::string sSend = jsonSend.dump();

		memset(&stValidate, 0, sizeof(stValidate));
		stValidate.m_nResult = 0;

		int ret = _request_with_serverurl(
			lv_szServerURL, "/api/auth/validate", lv_szJWT, sSend.c_str(), NULL,
			CALLBACK_VALIDATE, NULL, &stValidate, NULL, NULL);

		if (ret != 0 || stValidate.m_nResult != 0) {
			//.	failed connect
			strcpy_s(lv_szStatus, 30, "disconnected");
		}
		else {
			strcpy_s(lv_szStatus, 30, "connected");
			strcpy_s(lv_szJWT, MAX_PATH, stValidate.m_szToken);

			if (lv_fnCallbackValidate(
				lv_paramCallbackValidate, 
				stValidate.m_nValidate, 
				stValidate.m_szMessage, 
				stValidate.m_nSpeed, 
				stValidate.m_szHealth) == FALSE)
			{
				break;
			}
		}

		Sleep(10000);
	}
	return 0;
}

typedef struct tagUserDataOTP {
	char m_szToken[MAX_PATH];
	int m_nUserID;
	char m_szSID[128];
	int m_nResult;
}ST_USERDATA_OTP;
void CALLBACK_OTP(void* p_pJSON, void* p_pUserData) {
	json* pJson = (json*)p_pJSON;
	ST_USERDATA_OTP* pData = (ST_USERDATA_OTP*)p_pUserData;

	try {
		json data = (*pJson)["data"];
		std::string token = data["token"];
		strcpy_s(pData->m_szToken, MAX_PATH, token.c_str());
		json user = data["user"];
		pData->m_nUserID = user["id"];
		std::string sid = data["session_id"];
		strcpy_s(pData->m_szSID, 128, sid.c_str());
	}
	catch (std::exception e) {
		pData->m_nResult = -1;
	}
}
/// <summary>
/// Check OTP(One Time Password : 6 letters) - for client
/// </summary>
/// <param name="p_wszMachineID">(const wchar_t*)machine id: not used</param>
/// <param name="p_wszOTP">(const wchar_t*)OTP CODE</param>
/// <param name="p_wszIP">(const wchar_t*)Current IP Address</param>
/// <param name="p_fnCallBack">Callback, it will be called when success with speed, ...</param>
/// <param name="p_pParam"></param>
/// <returns>return 0 if success, otherwise non-zero</returns>
extern "C" __declspec(dllexport) unsigned int KC_OTP(
	const wchar_t* p_wszMachineID, 
	const wchar_t* p_wszOTP, 
	const wchar_t* p_wszIP,
	FN_CALLBACK_VALIDAT p_fnCallBack,
	void* p_pParam
) 
{
	ST_USERDATA_OTP stOTP; memset(&stOTP, 0, sizeof(stOTP));
	char szOtp[20]; memset(szOtp, 0, sizeof(szOtp));
	sprintf_s(szOtp, 20, "%S", p_wszOTP);
	memset(lv_szIP, 0, sizeof(lv_szIP));
	sprintf_s(lv_szIP, 64, "%S", p_wszIP);

	json jsonSend;
	jsonSend["__from_launcher__"] = 1;
	jsonSend["email"] = lv_szUser;
	jsonSend["password"] = lv_szPwd;
	jsonSend["machine_id"] = lv_szMachineID;
	jsonSend["os"] = lv_szOS;
	jsonSend["ip"] = lv_szIP;
	jsonSend["otp"] = szOtp;
	std::string sSend = jsonSend.dump();

	HANDLE hThread = _request_with_serverurl_thread(
		lv_szServerURL, "/api/auth/checkOTP", NULL, sSend.c_str(), NULL,
		CALLBACK_OTP, NULL, &stOTP, NULL, NULL);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);

	if (ret != 0) {
		return ret;
	}

	if (stOTP.m_nResult != 0) {
		return -100;
	}

	strcpy_s(lv_szJWT, MAX_PATH, stOTP.m_szToken);
	lv_nUserID = stOTP.m_nUserID;
	sprintf_s(lv_szUserID, 20, "%d", lv_nUserID);
	strcpy_s(lv_szSession, MAX_PATH, stOTP.m_szSID);

	lv_fnCallbackValidate = p_fnCallBack;
	lv_paramCallbackValidate = p_pParam;

	DWORD dwTID;
	lv_hThreadShake = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_SHAKE, NULL, 0, &dwTID);
	return 0;
}

typedef struct tagUserDataDropboxBearer {
	char* m_pToken;
	int cchToken;
	int m_nStatus;
}ST_USERDATA_DROPBOX_BEARER;
void CALLBACK_DROPBOX_BEARER(void* p_pJSON, void* p_pUserData) {
	json* pJson = (json*)p_pJSON;
	ST_USERDATA_DROPBOX_BEARER* pData = (ST_USERDATA_DROPBOX_BEARER*)p_pUserData;

	try {
		std::string token = (*pJson)["access_token"];
		strcpy_s(pData->m_pToken, pData->cchToken, token.c_str());
		pData->m_nStatus = 200;
	}
	catch (std::exception e) {
		pData->m_nStatus = 400;
	}
}
/// <summary>
///  Get bearer token from dropbox using refresh token
/// </summary>
/// <param name="p_szAppKey"></param>
/// <param name="p_szAppSecret"></param>
/// <param name="p_szRefreshToken"></param>
/// <param name="p_szBearerToken"></param>
/// <param name="cchBearerToken"></param>
/// <returns>return 0 if success, otherwise non-zero</returns>
/// <note>refresh token must be regenerate when permission was changed</note>
int _dropbox_get_bearer(
	const char* p_szAppKey,
	const char* p_szAppSecret,
	const char* p_szRefreshToken,
	char* p_szBearerToken,
	int cchBearerToken
) {
	ST_USERDATA_DROPBOX_BEARER stData;
	char szData[512]; memset(szData, 0, sizeof(szData));

	sprintf_s(szData, 512,
		"grant_type=refresh_token"
		"&refresh_token=%s"
		"&client_id=%s"
		"&client_secret=%s",
		p_szRefreshToken, p_szAppKey, p_szAppSecret);

	stData.m_pToken = p_szBearerToken;
	stData.cchToken = cchBearerToken;

	int nRet = _request_for_dropbox_info(
		"/oauth2/token",
		NULL,
		szData, TRUE,
		CALLBACK_DROPBOX_BEARER, &stData
	);
	if (nRet) return nRet;
	if (stData.m_nStatus != 200) return 400;
	return 0;
}

typedef struct tagUserDataDownVM {
	KC_CALLBACK_DOWN_VM callback;
	unsigned long long offset;

	tagUserDataDownVM() {
		callback = NULL;
		offset = 0;
	}
}ST_USERDATA_DOWN_VM;

unsigned long long lastDownload = -1;

BOOL CALLBACK_DOWNVM_PROGRESS(unsigned long long lTotal, unsigned long long lNow, void* p_pUserData)
{
	ST_USERDATA_DOWN_VM* pData = (ST_USERDATA_DOWN_VM*)p_pUserData;
	if (pData->callback && lTotal != 0) {
		Sleep(1);
		if (lNow - lastDownload > (1 << 20) || lNow == lTotal) {
			lastDownload = lNow;
			return pData->callback(lNow + pData->offset, lTotal + pData->offset);
		}
	}
	return TRUE;
}

/// <summary>
/// Download VM Image file from server - for client
/// </summary>
/// <param name="p_nID">VM Image ID</param>
/// <param name="p_wszDestination">File Full Path to save file</param>
/// <param name="p_fnCallback">Progress callback function</param>
/// <returns>return 0 if success, otherwise non-zero</returns>
extern "C" __declspec(dllexport) int KC_DOWN_VMIMAGE(
	const char* p_szDropboxPath,
	const wchar_t* p_wszDestination,
	BOOL p_bContinue,
	const char* p_szDropboxAppKey,
	const char* p_szDropboxAppSecret,
	const char* p_szDropboxRefreshToken,
	KC_CALLBACK_DOWN_VM p_fnCallback)
{
	int nRet = 0;
	unsigned long long lSize = 0;
	char szBearer[2048];
	ST_USERDATA_DOWN_VM stData;

	nRet = _dropbox_get_bearer(p_szDropboxAppKey, p_szDropboxAppSecret, p_szDropboxRefreshToken, szBearer, 2048);
	if (nRet != 0) {
		return nRet;
	}

	if (p_bContinue) {
		lSize = ClaPathMgr::GetFileSize(p_wszDestination);
		if (lSize < 0) lSize = 0;
	}

	stData.callback = p_fnCallback;
	stData.offset = lSize;

	nRet = _request_for_dropbox_download(szBearer, p_szDropboxPath, lSize, p_wszDestination, CALLBACK_DOWNVM_PROGRESS, &stData);

	return nRet;
}

typedef struct tagUserDataDownSize {
	unsigned long long *m_lSize;
	int m_nStatus;
}ST_USERDATA_DOWN_SIZE;
void CALLBACK_DOWN_VMIMAGE_SIZE(void* p, void* p_pUserData) {
	ST_USERDATA_DOWN_SIZE* pData = (ST_USERDATA_DOWN_SIZE*)p_pUserData;
	json* pJson = (json*)p;
	try {
		*pData->m_lSize = (*pJson)["size"];
		pData->m_nStatus = 200;
	}
	catch (std::exception e) {
		pData->m_nStatus = 400;
	}
}

/// <summary>
/// Download VM Image file from server - for client
/// </summary>
/// <param name="p_nID">VM Image ID</param>
/// <param name="p_wszDestination">File Full Path to save file</param>
/// <param name="p_fnCallback">Progress callback function</param>
/// <returns>return 0 if success, otherwise non-zero</returns>
extern "C" __declspec(dllexport) int KC_DOWN_VMIMAGE_SIZE(
	const char* p_szDropboxPath,
	const char* p_szDropboxAppKey,
	const char* p_szDropboxAppSecret,
	const char* p_szDropboxRefreshToken,
	unsigned long long *p_plSize
)
{
	int nRet = 0;
	unsigned long long lSize = 0;
	char szBearer[2048];
	ST_USERDATA_DOWN_VM stData;

	nRet = _dropbox_get_bearer(p_szDropboxAppKey, p_szDropboxAppSecret, p_szDropboxRefreshToken, szBearer, 2048);
	if (nRet != 0) {
		return nRet;
	}

	json jsonSend;
	jsonSend["path"] = p_szDropboxPath;
	std::string sSend = jsonSend.dump();

	ST_USERDATA_DOWN_SIZE stDownSize;
	stDownSize.m_lSize = p_plSize;

	nRet = _request_for_dropbox_info(
		"/2/files/get_metadata",
		szBearer, sSend.c_str(), FALSE, CALLBACK_DOWN_VMIMAGE_SIZE, &stDownSize);
	if (nRet != 0)
		return nRet;
	if (stDownSize.m_nStatus != 200)
		return 400;

	return 0;
}

extern "C" __declspec(dllexport) int KC_UP_VMIMAGE_STATUS(
	int p_nVmImageID,
	int p_nStatus
) {
	json jsonSend;
	jsonSend["__from_launcher__"] = 1;
	jsonSend["user_id"] = lv_nUserID;
	jsonSend["machine_id"] = lv_szMachineID;
	jsonSend["id"] = p_nVmImageID;
	jsonSend["status"] = p_nStatus;
	std::string sSend = jsonSend.dump();

	DWORD ret = _request_with_serverurl(
		lv_szServerURL, "/api/vm-image/download", lv_szJWT, sSend.c_str(), NULL,
		NULL, NULL, NULL, NULL, NULL);
	if (ret != 0) {
		return ret;
	}

	return 0;
}


void ReadVersion(ClaKcVersion* p_pVersion, json* pRoot, const char* key) {
	json group = *pRoot;

	if (group.contains(key) == false) return;
	json version_info = group[key];
	p_pVersion->_nID = version_info.contains("id") ? version_info["id"] : 0;
	p_pVersion->_action_type = version_info.contains("action_type") ? version_info["action_type"] : 0;
	if (version_info.contains("version")) {
		std::string version = version_info["version"];
		p_pVersion->_version = version;
	}
	if (version_info.contains("release_date")) {
		std::string release_date = version_info["release_date"];
		p_pVersion->_release_date = release_date;
	}
	if (version_info.contains("change_log")) {
		std::string change_log = version_info["change_log"];
		p_pVersion->_change_log = change_log;
	}
	if (version_info.contains("platform")) {
		std::string platform = version_info["platform"];
		p_pVersion->_platform = platform;
	}
}
void CALLBACK_DOWN_VERSION(void* p, void* p_pUserData) {
	ClaKcVersions* pData = (ClaKcVersions*)p_pUserData;
	ClaKcVersion* pLVersion = pData->launcher;
	ClaKcVersion* pAVersion = pData->agent;
	json* pJson = (json*)p;
	try {
		if (pJson->contains("data") == false) return;
		json root = (*pJson)["data"];
		json data = root["data"];
		ReadVersion(pLVersion, (json*)&data, "launcher");
		ReadVersion(pAVersion, (json*)&data, "agent");
	}
	catch (std::exception e) {
	}
}
/// <summary>
/// Get version information from server
/// </summary>
/// <returns>return class pointer to version if success, otherwise NULL</returns>
extern "C" __declspec(dllexport) ClaKcVersions* KC_DOWN_CHECK_UPDATE() {
	json jsonSend;
	jsonSend["__from_launcher__"] = 1;
	jsonSend["user_id"] = lv_nUserID;
	jsonSend["machine_id"] = lv_szMachineID;
	std::string sSend = jsonSend.dump();

	DWORD ret = _request_with_serverurl(
		lv_szServerURL, "/api/check-update", lv_szJWT, sSend.c_str(), NULL,
		CALLBACK_DOWN_VERSION, NULL, &lv_kcVersions, NULL, NULL);
	if (ret != 0) {
		return NULL;
	}

	return &lv_kcVersions;
}

/// <summary>
/// Download VM Image file from server - for client
/// </summary>
/// <param name="p_nID">VM Image ID</param>
/// <param name="p_wszDestination">File Full Path to save file</param>
/// <param name="p_fnCallback">Progress callback function</param>
/// <returns>return 0 if success, otherwise non-zero</returns>
extern "C" __declspec(dllexport) unsigned int KC_DOWN_UPDATE_FILE(int p_nID, const wchar_t* p_wszDestination)
{
	json jsonSend;
	jsonSend["__from_launcher__"] = 1;
	jsonSend["user_id"] = lv_nUserID;
	jsonSend["id"] = p_nID;
	jsonSend["machine_id"] = lv_szMachineID;
	std::string sSend = jsonSend.dump();

	HANDLE hThread = _request_with_serverurl_thread(
		lv_szServerURL, "/api/download-update", lv_szJWT, sSend.c_str(), NULL,
		NULL, NULL, NULL, NULL, p_wszDestination);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);

	if (ret != 0) {
		return ret;
	}

	return 0;
}

void ReadConfig(ClaKcGroup* p_pGroup, json* pRoot) {
	json group = *pRoot;

	if (group.contains("config") == false) return;
	json config = group["config"];
	p_pGroup->_config._bAllowTextToVM = config.contains("copy_text_to_vm") ? config["copy_text_to_vm"] : false;
	p_pGroup->_config._bAllowTextFromVM = config.contains("copy_text_from_vm") ? config["copy_text_from_vm"] : false;
	p_pGroup->_config._bAllowFileToVM = config.contains("copy_file_to_vm") ? config["copy_file_to_vm"] : false;
	p_pGroup->_config._bAllowFileFromVM = config.contains("copy_file_from_vm") ? config["copy_file_from_vm"] : false;
	p_pGroup->_config._bAllowCaptureScreen = config.contains("allow_screenshot") ? config["allow_screenshot"] : false;
	p_pGroup->_config._bCreateOutboundRule = config.contains("enable_outbound") ? config["enable_outbound"] : false;

	if (group.contains("ports") == false) return;
	json ports = group["ports"];
	for (const auto& port : ports) {
		ClaKcPort newPort;
		newPort._nSourcePort = 0;
		if (port.contains("source_port") && !port["source_port"].is_null()) {
			newPort._nSourcePort = port["source_port"];
		}
		else if (port.contains("listen_port") && !port["listen_port"].is_null()) {
			newPort._nSourcePort = port["listen_port"];
		}
		if (newPort._nSourcePort == 0) continue;

		if (port.contains("title") == false || port["title"].is_null()) continue;
		std::string title = port["title"]; strcpy_s(newPort._szTitle, 128, title.c_str());

		if (port.contains("target") == false || port["target"].is_null()) continue;
		std::string port_target = port["target"]; strcpy_s(newPort._szTarget, 128, port_target.c_str());

		if (port.contains("target_port") == false || port["target_port"].is_null()) continue;
		newPort._nTargetPort = port["target_port"];

		newPort._bIsHttps = (port.contains("is_https") && !port["is_https"].is_null()) ? port["is_https"] : FALSE;
		newPort._bIsDirect = (port.contains("is_https") && !port["is_https"].is_null()) ? port["is_direct"] : FALSE;

		p_pGroup->_lstPort.Add(newPort);
	}
}
void CALLBACK_DOWN_LIST(void* p, void* p_pUserData) {
	ClaKcPolicy* pPolicy = (ClaKcPolicy*)p_pUserData;
	json* pJson = (json*)p;
	try {
		if (pJson->contains("data") == false) return;
		json root = (*pJson)["data"];

		if (root.contains("groups") == false) return;
		json groups = root["groups"];

		for (const auto& group : groups) {
			ClaKcGroup newGroup;

			if (group.contains("id") == false) continue;
			int id = group["id"];
			if (group.contains("name") == false) continue;
			std::string name = group["name"];
			sprintf_s(newGroup._szID, 16, "%d", id);
			newGroup._id = id;
			strcpy_s(newGroup._szName, 64, name.c_str());

			ReadConfig(&newGroup, (json*)&group);

			if (group.contains("vm_images") == false) continue;
			json vm_images = group["vm_images"];
			for (const auto& image : vm_images) {
				ClaKcVmImage newImage;
				if (image.contains("id") == false) continue;
				int img_id = image["id"];
				sprintf_s(newImage._szID, 16, "%d", img_id);
				newImage._id = img_id;

				if (image.contains("title") == false) continue;
				std::string img_title = image["title"]; strcpy_s(newImage._szTitle, 128, img_title.c_str());

				if (image.contains("description") == false) continue;
				std::string img_desc = image["description"]; strcpy_s(newImage._szDesc, 256, img_desc.c_str());

				if (image.contains("password") == false) continue;
				std::string img_pwd = image["password"]; strcpy_s(newImage._szPwd, 64, img_pwd.c_str());

				if (image.contains("vm_os") == true) {
					std::string img_vm_os = image["vm_os"].is_null() ? "" : image["vm_os"]; strcpy_s(newImage._szOS, 128, img_vm_os.c_str());
				}
				if (image.contains("req_os") == true) {
					std::string img_req_os = image["req_os"].is_null() ? "" : image["req_os"]; strcpy_s(newImage._szReqOS, 128, img_req_os.c_str());
				}
				if (image.contains("req_memory") == true) {
					int req_memory = image["req_memory"].is_null() ? 0 : image["req_memory"]; newImage._nReqMemory = req_memory;
				}
				if (image.contains("req_free_space") == true) {
					int req_free_space = image["req_free_space"].is_null() ? 0 : image["req_free_space"]; newImage._nReqFreeSpace = req_free_space;
				}
				if (image.contains("req_cpu_count") == true) {
					int req_cpu_count = image["req_cpu_count"].is_null() ? 0 : image["req_cpu_count"]; newImage._nReqCpuCount = req_cpu_count;
				}
				if (image.contains("req_core_count_per_cpu") == true) {
					int req_core_count_per_cpu = image["req_core_count_per_cpu"].is_null() ? 0 : image["req_core_count_per_cpu"]; newImage._nReqCoreCount = req_core_count_per_cpu;
				}
				if (image.contains("req_antivirus") == true) {
					std::string req_antivirus = image["req_antivirus"].is_null() ? "" : image["req_antivirus"]; strcpy_s(newImage._szReqAntiVirus, 128, req_antivirus.c_str());
				}
				if (image.contains("download_url") == true) {
					std::string data = image["download_url"].is_null() ? "" : image["download_url"]; strcpy_s(newImage._szDropboxVmPath, MAX_PATH, data.c_str());
				}

				if (image.contains("dropbox_app_key") == true) {
					std::string data = image["dropbox_app_key"].is_null() ? "" : image["dropbox_app_key"]; strcpy_s(newImage._szDropboxAppKey, 20, data.c_str());
				}
				if (image.contains("dropbox_app_secret") == true) {
					std::string data = image["dropbox_app_secret"].is_null() ? "" : image["dropbox_app_secret"]; strcpy_s(newImage._szDropboxAppSecret, 20, data.c_str());
				}
				if (image.contains("dropbox_refresh_token") == true) {
					std::string data = image["dropbox_refresh_token"].is_null() ? "" : image["dropbox_refresh_token"]; strcpy_s(newImage._szDropboxAppRefresgToken, 70, data.c_str());
				}

				if (image.contains("size") == true) {
					newImage._lSize = image["size"];
				}
				else {
					newImage._lSize = 0;
				}

				newGroup._lstVmImage.Add(newImage);
			}

			pPolicy->_lstGroup.Add(newGroup);
		}

		if (root.contains("agent_timeout")) {
			pPolicy->agent_timeout = root["agent_timeout"];
		}
		if (root.contains("session_expircy_time")) {
			pPolicy->session_expircy_time = root["session_expircy_time"];
		}
	}
	catch (std::exception e) {
	}
}
/// <summary>
/// download group and vm list from server
/// </summary>
extern "C" __declspec(dllexport) ClaKcPolicy* KC_DOWN_LIST() {
	json jsonSend;
	jsonSend["__from_launcher__"] = 1;
	jsonSend["user_id"] = lv_nUserID;
	std::string sSend = jsonSend.dump();

	lv_kcPolicy._lstGroup.RemoveAll();

	HANDLE hThread = _request_with_serverurl_thread(
		lv_szServerURL, "/api/user/config", lv_szJWT, sSend.c_str(), NULL,
		CALLBACK_DOWN_LIST, NULL, &lv_kcPolicy, NULL, NULL);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);

	if (ret != 0) {
		return NULL;
	}

	return &lv_kcPolicy;
}

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
std::time_t convertToTimeT(const std::string& timestamp) {
	std::tm tm = {};
	std::istringstream ss(timestamp);

	// Parse the timestamp

	//12/26/2024 22:15:00
	ss >> std::get_time(&tm, "%m/%d/%Y %H:%M:%S");
	if (ss.fail()) {
		throw std::runtime_error("Failed to parse timestamp");
	}

	// Convert std::tm to std::time_t
	return std::mktime(&tm);
}

void ReadScripts(ClaKcGroup* p_pGroup, json* pRoot) {
	json group = *pRoot;

	p_pGroup->_lstViolation.RemoveAll();
	if (group.contains("violations")) {
		json violations_list = group["violations"];

		for (const auto& json_violation : violations_list) {
			ClaKcViolation violation;

			if (json_violation.contains("id")) {
				violation._nID = json_violation["id"];
			}
			if (json_violation.contains("title")) {
				std::string title = json_violation["title"];
				violation._title = title;
			}
			if (json_violation.contains("action1")) {
				violation._action1 = json_violation["action1"];
			}
			if (json_violation.contains("action2")) {
				violation._action2 = json_violation["action2"];
			}
			if (json_violation.contains("os")) {
				std::string os = json_violation["os"];
				violation._os = os;
			}
			if (json_violation.contains("check_at_start")) {
				violation._check_at_start = (int)json_violation["check_at_start"];
			}
			if (json_violation.contains("check_interval")) {
				violation._check_interval = json_violation["check_interval"];
			}
			if (json_violation.contains("message")) {
				std::string message = json_violation["message"];
				violation._message = message;
			}
			if (json_violation.contains("show_warning")) {
				violation._show_warning = (int)json_violation["show_warning"];
			}

			if (json_violation.contains("policy_scripts")) {
				json policy_script_list = json_violation["policy_scripts"];
				for (const auto& policy_script : policy_script_list) {
					ClaKcPolicyScript policyScript;
					if (policy_script.contains("id")) {
						policyScript._nID = policy_script["id"];
					}
					if (policy_script.contains("title")) {
						std::string title = policy_script["title"];
						policyScript._title = title;
					}
					if (policy_script.contains("script")) {
						std::string script = policy_script["script"];
						policyScript._script = script;
					}
					if (policy_script.contains("expected_result")) {
						std::string expected_result = policy_script["expected_result"];
						policyScript._expected_result = expected_result;
					}
					if (policy_script.contains("os")) {
						std::string os = policy_script["os"];
						policyScript._os = os;
					}
					violation._policy_scripts.Add(policyScript);
				}
			}
			p_pGroup->_lstViolation.Add(violation);
		}
	}

	p_pGroup->_lstAgentScriptPlan.RemoveAll();
	if (group.contains("agent_script_plans")) {
		json agent_script_plans = group["agent_script_plans"];

		for (const auto& json_agent_script_plan : agent_script_plans) {
			ClaKcAgentScriptPlan agent_script_plan;
			if (json_agent_script_plan.contains("id")) {
				agent_script_plan._nID = json_agent_script_plan["id"];
			}
			if (json_agent_script_plan.contains("agent_script_id")) {
				agent_script_plan._agent_script_id = json_agent_script_plan["agent_script_id"];
			}
			if (json_agent_script_plan.contains("title")) {
				std::string agent_script_title = json_agent_script_plan["title"];
				agent_script_plan._agent_script_title = agent_script_title;
			}
			if (json_agent_script_plan.contains("agent_script")) {
				std::string agent_script = json_agent_script_plan["agent_script"];
				agent_script_plan._agent_script = agent_script;
			}
			if (json_agent_script_plan.contains("os")) {
				std::string os = json_agent_script_plan["os"];
				agent_script_plan._os = os;
			}
			if (json_agent_script_plan.contains("execution_type")) {
				agent_script_plan._execution_type = json_agent_script_plan["execution_type"];
			}
			if (json_agent_script_plan.contains("scheduled_time")) {
				try {
					std::string schedule = json_agent_script_plan["scheduled_time"];;
					agent_script_plan._scheduled_time = convertToTimeT(schedule);
				}
				catch (std::exception e) {
					agent_script_plan._scheduled_time = 0;
				}
			}
			if (json_agent_script_plan.contains("run_at_once")) {
				agent_script_plan._run_at_once = (int)json_agent_script_plan["run_at_once"];
			}
			p_pGroup->_lstAgentScriptPlan.Add(agent_script_plan);
		}
	}
}
void CALLBACK_DOWN_CONFIG(void* p, void* p_pUserData) {
	ClaKcGroup* pGroup = (ClaKcGroup*)p_pUserData;
	json* pJson = (json*)p;
	try {
		if (pJson->contains("data") == false) return;
		json root = (*pJson)["data"];

		ReadConfig(pGroup, (json*)&root);
		ReadScripts(pGroup, (json*)&root);
	}
	catch (std::exception e) {
	}
}
/// <summary>
/// Download config for group->image from server
/// </summary>
/// <param name="p_nGroupID"></param>
/// <param name="p_nImageID"></param>
/// <returns></returns>
extern "C" __declspec(dllexport) ClaKcGroup* KC_DOWN_CONFIG(int p_nGroupID, int p_nImageID) {
	//"{\"__from_launcher__\":1,\"user_id\":%s,\"group_id\":%d,\"vm_image_id\":%d,\"machine_id\":\"%s\"}"
	json jsonSend;
	jsonSend["__from_launcher__"] = 1;
	jsonSend["user_id"] = lv_nUserID;
	jsonSend["group_id"] = p_nGroupID;
	jsonSend["vm_image_id"] = p_nImageID;
	jsonSend["machine_id"] = lv_szMachineID;
	std::string sSend = jsonSend.dump();

	lv_kcGroup._lstAgentScriptPlan.RemoveAll();
	lv_kcGroup._lstPort.RemoveAll();
	lv_kcGroup._lstViolation.RemoveAll();

	HANDLE hThread = _request_with_serverurl_thread(
		lv_szServerURL, "/api/user/check_config", lv_szJWT, sSend.c_str(), NULL,
		CALLBACK_DOWN_CONFIG, NULL, &lv_kcGroup, NULL, NULL);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);

	if (ret != 0) {
		return NULL;
	}

	return &lv_kcGroup;
}

extern "C" __declspec(dllexport) int KC_SET_PASSWORD(int p_nImageID, const char* p_szKey) {
	json jsonSend;
	jsonSend["__from_launcher__"] = 1;
	jsonSend["user_id"] = lv_nUserID;
	jsonSend["id"] = p_nImageID;
	jsonSend["machine_id"] = lv_szMachineID;
	jsonSend["password"] = p_szKey;
	std::string sSend = jsonSend.dump();

	HANDLE hThread = _request_with_serverurl_thread(
		lv_szServerURL, "/api/vm-image/set-password", lv_szJWT, sSend.c_str(), NULL,
		NULL, NULL, NULL, NULL, NULL);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);

	return ret;
}

typedef struct tagReqGetPassword {
	char* pVmPassword;
	int cchVmPassword;
	char* pEncPassword;
	int cchEncPassword;
}ST_REQ_GET_PASSWORD;
void CALLBACK_GET_PASSWORD(void* p, void* p_pUserData) {
	json* pJson = (json*)p;
	ST_REQ_GET_PASSWORD* pParam = (ST_REQ_GET_PASSWORD*)p_pUserData;

	try {
		if (pJson->contains("data") == false) return;
		json root = (*pJson)["data"];
		json data = root["data"];

		if (data.contains("vmimage_password")) {
			std::string vmimage_password = data["vmimage_password"];
			strcpy_s(pParam->pVmPassword, pParam->cchVmPassword, vmimage_password.c_str());
		}
		if (data.contains("enc_password")) {
			std::string enc_password = data["enc_password"];
			strcpy_s(pParam->pEncPassword, pParam->cchEncPassword, enc_password.c_str());
		}
	}
	catch (std::exception e) {
	}
}
extern "C" __declspec(dllexport) int KC_GET_PASSWORD(int p_nImageID, char* p_szKey, int p_cchKey, char* p_szPwd, int p_cchPwd) {
	ST_REQ_GET_PASSWORD stReqGetPassword;

	json jsonSend;
	jsonSend["__from_launcher__"] = 1;
	jsonSend["user_id"] = lv_nUserID;
	jsonSend["id"] = p_nImageID;
	jsonSend["machine_id"] = lv_szMachineID;
	std::string sSend = jsonSend.dump();

	stReqGetPassword.pVmPassword = p_szKey;
	stReqGetPassword.cchVmPassword = p_cchKey;
	stReqGetPassword.pEncPassword = p_szPwd;
	stReqGetPassword.cchEncPassword = p_cchPwd;

	HANDLE hThread = _request_with_serverurl_thread(
		lv_szServerURL, "/api/vm-image/get-password", lv_szJWT, sSend.c_str(), NULL,
		CALLBACK_GET_PASSWORD, NULL, &stReqGetPassword, NULL, NULL);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);

	return ret;
}

extern "C" __declspec(dllexport) unsigned int KC_REPORT_ACTION(
	int p_nVmImageID,
	const char* p_szAction)
{
	json jsonSend;
	jsonSend["__from_launcher__"] = 1;
	jsonSend["user_id"] = lv_nUserID;
	jsonSend["vmimage_id"] = p_nVmImageID;
	jsonSend["machine_id"] = lv_szMachineID;
	jsonSend["action"] = p_szAction;
	std::string sSend = jsonSend.dump();

	HANDLE hThread = _request_with_serverurl_thread(
		lv_szServerURL, "/api/vm-image/report-action", lv_szJWT, sSend.c_str(), NULL,
		NULL, NULL, NULL, NULL, NULL);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);

	return ret;
}

extern "C" __declspec(dllexport) unsigned int KC_REPORT_VIOLATION(
	int vmimage_id,
	int violation_id,
	int script_id,
	int action1_done,
	int action2_done,
	const char* report)
{
	json jsonSend;
	jsonSend["__from_launcher__"] = 1;
	jsonSend["user_id"] = lv_nUserID;
	jsonSend["vmimage_id"] = vmimage_id;
	jsonSend["machine_id"] = lv_szMachineID;
	jsonSend["session_id"] = lv_szSession;
	jsonSend["violation_id"] = violation_id;
	jsonSend["violation_script_id"] = script_id;
	jsonSend["action1_done"] = action1_done;
	jsonSend["action2_done"] = action2_done;
	jsonSend["report"] = report;
	std::string sSend = jsonSend.dump();

	HANDLE hThread = _request_with_serverurl_thread(
		lv_szServerURL, "/api/violation/report", lv_szJWT, sSend.c_str(), NULL,
		NULL, NULL, NULL, NULL, NULL);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);

	return ret;
}

extern "C" __declspec(dllexport) unsigned int KC_REPORT_AGENT_SCRIPT(
	int vmimage_id,
	int script_id,
	int status,
	const char* report)
{
	json jsonSend;
	jsonSend["__from_launcher__"] = 1;
	jsonSend["user_id"] = lv_nUserID;
	jsonSend["vmimage_id"] = vmimage_id;
	jsonSend["machine_id"] = lv_szMachineID;
	jsonSend["agent_script_plan_id"] = script_id;
	jsonSend["status"] = status;
	jsonSend["report"] = report;
	std::string sSend = jsonSend.dump();

	HANDLE hThread = _request_with_serverurl_thread(
		lv_szServerURL, "/api/agent-script-plan/report", lv_szJWT, sSend.c_str(), NULL,
		NULL, NULL, NULL, NULL, NULL);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);

	return ret;
}



std::string XORCipher(const std::string& input, const std::string& key) {
	std::string output = input; // Start with the input string

	// Perform XOR operation
	for (size_t i = 0; i < input.size(); ++i) {
		output[i] = input[i] ^ key[i % key.size()]; // XOR with the key, cycling through the key
	}

	return output;
}

extern "C" __declspec(dllexport) int KC_UP_PORT(const char* p_szIP, const char* p_szData) {
	char szServer[MAX_PATH]; memset(szServer, 0, sizeof(szServer));
	sprintf_s(szServer, MAX_PATH, "http://%s:18080", p_szIP);

	std::string c = XORCipher(p_szData, "__kenan_enckey_20241114");
	std::string b64 = base64_encode(c);

	HANDLE hThread = _request_with_serverurl_thread(
		szServer, "/api/port_getinfo__", NULL, b64.c_str(), NULL,
		NULL, NULL, NULL, NULL, NULL);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);

	return ret;
}

//set violation to agent
extern "C" __declspec(dllexport) int KC_UP_VIOLATION(const char* p_szIP, const char* p_szData) {
	char szServer[MAX_PATH]; memset(szServer, 0, sizeof(szServer));
	sprintf_s(szServer, MAX_PATH, "http://%s:18080", p_szIP);

	std::string c = XORCipher(p_szData, "__kenan_enckey_20241114");
	std::string b64 = base64_encode(c);

	HANDLE hThread = _request_with_serverurl_thread(
		szServer, "/api/violation_getinfo__", NULL, b64.c_str(), NULL,
		NULL, NULL, NULL, NULL, NULL);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);

	return ret;
}

//set script to agent
extern "C" __declspec(dllexport) int KC_UP_AGENT_SCRIPT(const char* p_szIP, const char* p_szData) {
	char szServer[MAX_PATH]; memset(szServer, 0, sizeof(szServer));
	sprintf_s(szServer, MAX_PATH, "http://%s:18080", p_szIP);

	std::string c = XORCipher(p_szData, "__kenan_enckey_20241114");
	std::string b64 = base64_encode(c);

	HANDLE hThread = _request_with_serverurl_thread(
		szServer, "/api/agentscript_getinfo__", NULL, b64.c_str(), NULL,
		NULL, NULL, NULL, NULL, NULL);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);

	return ret;
}

typedef struct tagUserDataUpIP
{
	char *m_szOS;
	int m_cchOS;
	char *m_szMAC;
	int m_cchMAC;
	int m_nResult;
}ST_USERDATA_UP_IP;
void CALLBACK_UP_IP(void* p, void* p_pUserData) {
	ST_USERDATA_UP_IP* pData = (ST_USERDATA_UP_IP *)p_pUserData;
	json* pJson = (json*)p;

	try {
		pData->m_nResult = 0;
		if ((*pJson).contains("os")) {
			std::string os = (*pJson)["os"];
			strcpy_s(pData->m_szOS, pData->m_cchOS, os.c_str());
		}
		else {
			strcpy_s(pData->m_szOS, pData->m_cchOS, "unknown");
		}
		if ((*pJson).contains("mac")) {
			std::string mac = (*pJson)["mac"];
			strcpy_s(pData->m_szMAC, pData->m_cchMAC, mac.c_str());
		}
		else {
			strcpy_s(pData->m_szMAC, pData->m_cchMAC, "unknown");
		}
	}
	catch (std::exception e) {
		pData->m_nResult = -1;
	}
}
extern "C" __declspec(dllexport) int KC_UP_IP(const char* p_szIP, const char* p_szData, char* p_szOS, int cchOS, char* p_szMAC, int cchMAC) {
	char szServer[MAX_PATH]; memset(szServer, 0, sizeof(szServer));
	sprintf_s(szServer, MAX_PATH, "http://%s:18080", p_szIP);

	ST_USERDATA_UP_IP stUserData; memset(&stUserData, 0, sizeof(stUserData));
	stUserData.m_szOS = p_szOS;
	stUserData.m_szMAC= p_szMAC;
	stUserData.m_cchOS = cchOS;
	stUserData.m_cchMAC = cchMAC;

	std::string c = XORCipher(p_szData, "__kenan_enckey_20241114");
	std::string b64 = base64_encode(c);

	HANDLE hThread = _request_with_serverurl_thread(
		szServer, "/api/ip_getinfo__", NULL, b64.c_str(), NULL,
		CALLBACK_UP_IP, NULL, &stUserData, NULL, NULL, 2);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);

	return ret;
}

typedef struct tagUserDataGetIdle
{
	int* pIdle;
	int* pViolationID;
	int* pScriptID;
	char* szViolation;//[512]
	char* szScript;//[2048]

	int m_nResult;
}ST_USERDATA_GET_IDLE;
void CALLBACK_GETIDLE(void* p, void* p_pUserData) {
	ST_USERDATA_GET_IDLE* pData = (ST_USERDATA_GET_IDLE*)p_pUserData;
	json* pJson = (json*)p;

	try {
		json root = (*pJson)["data"]["data"];

		try {
			if (pData->pIdle)
				*pData->pIdle = root["idle"];
			if (root.contains("violation_id"))
				*pData->pViolationID = root["violation_id"];
			if (root.contains("script_id"))
				*pData->pScriptID = root["script_id"];

			if (root.contains("violation")) {
				std::string result = root["violation"];
				strncpy_s(pData->szViolation, 512, result.c_str(), 511);
			}
			if (root.contains("agent_script")) {
				std::string result = root["agent_script"];
				strncpy_s(pData->szScript, 2048, result.c_str(), 2047);
			}
			if (root.contains("qr_state")) {
				lv_qr_state = root["qr_state"];
			}
		}
		catch (std::exception e) {
			pData->m_nResult = -1;
		}

	}
	catch (std::exception e) {
		pData->m_nResult = -2;
	}
}
extern "C" __declspec(dllexport) int KC_GET_IDLE(const char* p_szIP, int* p_pnIdle, char* scriptResult,int* violationID, int* scriptID, char* agentScripts) {
	char szServer[MAX_PATH]; memset(szServer, 0, sizeof(szServer));
	sprintf_s(szServer, MAX_PATH, "http://%s:18080", p_szIP);

	ST_USERDATA_GET_IDLE stUserData; memset(&stUserData, 0, sizeof(stUserData));
	stUserData.pIdle = p_pnIdle;
	stUserData.pViolationID = violationID;
	stUserData.pScriptID = scriptID;
	stUserData.szViolation = scriptResult;
	stUserData.szScript = agentScripts;

	HANDLE hThread = _request_with_serverurl_thread(
		szServer, "/api/request_idle", NULL, "{}", NULL,
		CALLBACK_GETIDLE, NULL, &stUserData, NULL, NULL, 1);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);
	if (ret) return ret;
	if (stUserData.m_nResult != 0) return -100;

	return 0;
}

typedef struct tagUserDataAgentVersion {
	char* pszAgentVersion;//[20]
	int m_nResult;
}ST_USERDATA_AGENT_VERSION;
void CALLBACK_AGENT_VERSION(void* p, void* p_pUserData) {
	ST_USERDATA_AGENT_VERSION* pData = (ST_USERDATA_AGENT_VERSION*)p_pUserData;
	json* pJson = (json*)p;

	try {
		json root = (*pJson)["data"];

		try {
			if (root.contains("version")) {
				std::string version = root["version"];
				strcpy_s(pData->pszAgentVersion, 20, version.c_str());
			}
		}
		catch (std::exception e) {
			pData->m_nResult = -1;
		}
	}
	catch (std::exception e) {
		pData->m_nResult = -2;
	}
}
extern "C" __declspec(dllexport) int KC_GET_AGENT_VERSION(const char* p_szIP, char* p_szVersion) {
	char szServer[MAX_PATH]; memset(szServer, 0, sizeof(szServer));
	sprintf_s(szServer, MAX_PATH, "http://%s:18080", p_szIP);

	ST_USERDATA_AGENT_VERSION stData; memset(&stData, 0, sizeof(stData));
	stData.pszAgentVersion = p_szVersion;

	HANDLE hThread = _request_with_serverurl_thread(
		szServer, "/api/request_idle", NULL, "{}", NULL,
		CALLBACK_AGENT_VERSION, NULL, &stData, NULL, NULL);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);
	if (ret) return ret;
	if (stData.m_nResult != 0) return -100;

	return 0;
}

extern "C" __declspec(dllexport) unsigned int KC_UPLOAD_UPDATE(const char* p_szIP, const wchar_t* p_fileUrl)
{
	char szServer[MAX_PATH]; memset(szServer, 0, sizeof(szServer));
	sprintf_s(szServer, MAX_PATH, "http://%s:18080", p_szIP);

	HANDLE hThread = _request_with_serverurl_thread(
		szServer, "/api/upload_update", NULL, NULL, p_fileUrl,
		NULL, NULL, NULL, NULL, NULL);
	WaitForSingleObject(hThread, INFINITE);

	DWORD ret = -1;
	GetExitCodeThread(hThread, &ret);
	
	return ret;
}

char lv_szGIP[30];
size_t WriteCallback_IP(void* contents, size_t size, size_t nmemb, char* userp) {
	if (nmemb < 16)
		memcpy(userp, contents, nmemb);
	return size * nmemb;
}

extern "C" __declspec(dllexport) const char* KC_GET_GLOBAL_IP()
{
	CURL* curl;
	CURLcode res;
	memset(lv_szGIP, 0, sizeof(lv_szGIP));

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://api.ipify.org");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback_IP);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, lv_szGIP);

		// Disable SSL verification (not recommended for production)
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

	return lv_szGIP;
}

extern "C" __declspec(dllexport) void KC_RELEASE()
{
}