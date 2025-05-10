#include "MIServer.h"
#include "MiParserJson.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <process.h> 
#include <shellapi.h>
#include <string>
#include <locale>
#include <codecvt>
#include "base64enc.h"
#include <tlhelp32.h>
#include "ClaPowerShellMgr.h"

#include <iphlpapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "iphlpapi.lib") // Link with iphlpapi.lib
#pragma comment(lib, "ws2_32.lib")   // Link with Winsock library

#pragma comment(lib, "ntdll.lib")

typedef LONG NTSTATUS;
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

// Function prototype for RtlGetVersion
extern "C" NTSTATUS NTAPI RtlGetVersion(POSVERSIONINFOEX lpVersionInformation);

#define LD_MAX_TRIAL_COUNT 100
int lv_nTrialCount = 50 * 2;
std::string		lv_sKenanLanucher = "192.168.144.137";
std::string		lv_sGlobalIP = "192.168.144.137";

std::vector<ViolationRule> lv_violationRulesTemp;
std::vector<AgentScriptRule> lv_agentScriptRulesTemp;
std::vector<ViolationRule> lv_violationRules;
std::vector<AgentScriptRule> lv_agentScriptRules;
BOOL			lv_bQrFinished = FALSE;
long long lv_lastConnTime = -1;


CRITICAL_SECTION violation_section;

int _execute(const wchar_t* p_7zipExePath, const wchar_t* p_command, int	p_bWait = 1);
std::string getOS();

//wchar_t wszLog[4096];

void logFmt(LPCWSTR fmt, ...) {
	//vswprintf_s(wszLog, 4096, fmt, vl);
	//OutputDebugString(wszLog);
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));

	GetModuleFileNameW(NULL, wszPath, MAX_PATH);
	wchar_t* pPos = wcsrchr(wszPath, L'\\');
	pPos[0] = 0;
	wcscat_s(wszPath, L"\\agent.log");

	FILE* pFile; _wfopen_s(&pFile, wszPath, L"ab");
	wchar_t wszFmt[MAX_PATH]; 
	swprintf_s(wszFmt, L"%s\n", fmt);

	va_list vl;
	va_start(vl, fmt);
	if (pFile) {
		vfwprintf(pFile, wszFmt, vl);
		fclose(pFile);
	}
	va_end(vl);
}
UINT TF_COPY_SCRIPT_RULE(void* p)
{
	int idx = (int)p;
	EnterCriticalSection(&violation_section);
	if (idx == 1)
	{
		lv_violationRulesTemp.clear();
		for (ViolationRule& violation : lv_violationRules) {
			lv_violationRulesTemp.push_back(violation);
		}
	}

	if (idx == 2)
	{
		lv_agentScriptRulesTemp.clear();
		for (AgentScriptRule& scriptRule : lv_agentScriptRules) {
			lv_agentScriptRulesTemp.push_back(scriptRule);
		}
	}
	LeaveCriticalSection(&violation_section);

	return 0;
}

#include <iostream>
#include <ctime>
#include <string>
#include <iomanip>

time_t StringToTimeT(const std::string& dateTimeStr, const std::string& format) {
	std::tm tm = {};
	std::istringstream ss(dateTimeStr);
	ss >> std::get_time(&tm, format.c_str());

	if (ss.fail()) {
		throw std::runtime_error("Failed to parse date/time string");
	}

	// Convert std::tm to time_t
	return std::mktime(&tm);
}


UINT TF_VIOLATION(void*) {
	logFmt(L"SfT - start TF_VIOLATION\n");
	InitializeCriticalSection(&violation_section);
	std::string current_os = getOS();
	while (true) {
		EnterCriticalSection(&violation_section);

		for (ViolationRule& violation : lv_violationRulesTemp) {
			BOOL bRun = false;
			if (violation.check_at_start) {
				if (violation.last_time == 0) {
					bRun = true;
				}
			}
			else if (violation.check_interval > 0 && (GetTickCount() - violation.last_time) > (violation.check_interval)) {
				bRun = true;
			}

			logFmt(L"SfT - start TF_VIOLATION %d - %d", violation.id, bRun);

			if (bRun) {
				for (auto& script : violation.scripts) {
					if (script.os.find(current_os) == std::string::npos) continue;
					logFmt(L"SfT - TF_VIOLATION run-violation - %S", script.script.c_str());
					std::string ret = RunPowerShellScript(script.script.c_str());
					logFmt(L"SfT - TF_VIOLATION run-violation - %S", script.result.c_str());
					script.result = ret;
				}
				violation.last_time = GetTickCount();
			}
		}
		for (AgentScriptRule& agentScript : lv_agentScriptRulesTemp) {
			if (agentScript.os.find(current_os) == std::string::npos) continue;
			BOOL bRun = false;
			if (agentScript.status != 0) {
				bRun = false;
				continue;
			}

			if (agentScript.run_at_once) {
				bRun = true;
			}
			else {
				// Get the current time as a time_point
				time_t now_c = time(nullptr);
				time_t schedule = StringToTimeT(agentScript.scheduled_time, "%Y-%m-%d %H:%M:%S");

				int period = now_c - schedule;
				if (period < 60 && period > -60)
					bRun = true;
			}

			logFmt(L"SfT - TF_VIOLATION scriptplan - %S-%d", agentScript.agent_script.c_str(), bRun);

			if (bRun) {
				do {
					if (agentScript.execution_type == 1) {
						wchar_t wszDir[MAX_PATH]; memset(wszDir, 0, sizeof(wszDir));
						wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
						GetModuleFileName(NULL, wszPath, MAX_PATH);
						wchar_t* pPos = wcsrchr(wszPath, L'\\');
						pPos[0] = 0;
						wcscpy_s(wszDir, MAX_PATH, wszPath);
						wcscat_s(wszPath, L"\\KAgentQR.exe");

						STARTUPINFO si;
						PROCESS_INFORMATION pi;
						ZeroMemory(&si, sizeof(si));
						si.cb = sizeof(si);
						CreateProcessW(wszPath, (LPWSTR)L"--alert", NULL, NULL, FALSE, 0, NULL, wszDir, &si, &pi);
						WaitForSingleObject(pi.hProcess, INFINITE);
						DWORD dwExitCode = 0; GetExitCodeProcess(pi.hProcess, &dwExitCode);

						if (dwExitCode != 500) {
							agentScript.status = 3;
							agentScript.notified = false;
							break;
						}
					}
					agentScript.status = 1;
					agentScript.result = RunPowerShellScript(agentScript.agent_script.c_str());
					logFmt(L"SfT - TF_VIOLATION scriptplan - %S", agentScript.result.c_str());
					agentScript.last_time = GetTickCount();
					agentScript.notified = false;
				} while (false);
			}
		}
		LeaveCriticalSection(&violation_section);
		Sleep(300);
	}
	return 0;
}

std::string trim_all(std::string org, const std::string& trimletters) {
	// Use lambda to define a function for trimming characters
	auto trim = [&trimletters](std::string& str) {
		str.erase(0, str.find_first_not_of(trimletters)); // Trim from the start
		str.erase(str.find_last_not_of(trimletters) + 1); // Trim from the end
		};

	trim(org); // Trim the original string
	return org; // Return the trimmed string
}

std::string replace_all(std::string org, const std::string& from, const std::string& to) {
	if (from.empty()) {
		return org; // Return original string if 'from' is empty
	}

	size_t start_pos = 0;
	while ((start_pos = org.find(from, start_pos)) != std::string::npos) {
		org.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Move past the last replacement
	}
	return trim_all(org, " ");
}

std::string GetViolation(int & p_nViolationID,int& p_nScriptID) {
	std::string ret;
	for (auto& violation : lv_violationRulesTemp) {
		for (auto& script : violation.scripts) {
			std::string result = replace_all(script.result, "\r\n", " ");
			std::string expect = replace_all(script.expect, "\r\n", " ");
			if (result != expect) {
				p_nViolationID = violation.id;
				p_nScriptID = script.id;
				ret = script.result;
				script.result = script.expect;
				LeaveCriticalSection(&violation_section);
				return ret;
			}
		}
	}
	return "";
}
std::string GetAgentScript() {
	Poco::JSON::Array::Ptr list = new Array;
	for (auto& agentScript : lv_agentScriptRulesTemp) {
		if (agentScript.notified == true) continue;

		Poco::JSON::Object::Ptr item = new Poco::JSON::Object;
		item->set("id", agentScript.id);
		item->set("status", agentScript.status);
		item->set("report", agentScript.result.c_str());
		logFmt(L"script-report = [%d][%S]", agentScript.id, agentScript.result.c_str());

		list->add(item);

		agentScript.notified = true;
	}
	std::ostringstream oss;
	Stringifier::stringify(list, oss);
	std::string ret = oss.str();
	return ret;
}
std::string XORCipher(const std::string& input, const std::string& key) {
	std::string output = input; // Start with the input string

	// Perform XOR operation
	for (size_t i = 0; i < input.length(); ++i) {
		output[i] = input[i] ^ key[i % key.size()]; // XOR with the key, cycling through the key
	}

	return output;
}


std::string replaceAll(std::string original, const std::string& search, const std::string& replace) {
	size_t pos = 0;
	while ((pos = original.find(search, pos)) != std::string::npos) {
		original.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return original;
}

HTTPResponse::HTTPStatus sendPostRequest(const std::string& url, const std::string& payload, std::string& p_strRsp)
{
	URI uri(url);
	HTTPClientSession session(uri.getHost(), uri.getPort());

	HTTPRequest request(HTTPRequest::HTTP_POST, uri.getPathAndQuery(), HTTPMessage::HTTP_1_1);
	request.setContentType("application/json");
	request.setContentLength(payload.length());

	std::ostream& os = session.sendRequest(request);
	os << payload;

	HTTPResponse response;
	std::istream& rs = session.receiveResponse(response);

	std::string responseData;
	StreamCopier::copyToString(rs, responseData);
	p_strRsp = responseData;

	return response.getStatus();

}

void ClaHTTPServerWrapper::launch() {
	run();
}
void MyRequestHandler::OnUnknown(HTTPServerRequest& request, HTTPServerResponse& response)
{
	response.setStatus(HTTPResponse::HTTP_OK);
	response.setContentType("text/plain");

	response.set("Access-Control-Allow-Origin", "*");
	response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
	response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

	ostream& ostr = response.send();
	ostr << "Not found";
}
bool saveBinaryToFile(const std::string& filename, const std::string& data) {
	DeleteFileA(filename.c_str());
	std::ofstream outFile(filename, std::ios::binary);
	if (!outFile) {
		std::cerr << "Error opening file for writing: " << filename << std::endl;
		return false;
	}
	outFile.write(data.c_str(), data.size());
	outFile.close();
	return true;
}

int _execute(const wchar_t* p_szEXE, const wchar_t* p_pszCommandParam, int	p_bWait /*= 1*/)
{
	SHELLEXECUTEINFO ShExecInfo;
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = L"open";
	ShExecInfo.lpFile = p_szEXE;
	ShExecInfo.lpParameters = p_pszCommandParam; //  L"/C apktool.bat -f d D:\\work\\_FCM\\test_org.apk -o D:\\work\\_FCM\\aaa";
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE; // SW_NORMAL

	if (ShellExecuteEx(&ShExecInfo)) {
		// Wait for the process to exit
		if (p_bWait == 1) {
			WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
		}
		else {
			Sleep(1000);
		}
		DWORD dwCode = 0;
		GetExitCodeProcess(ShExecInfo.hProcess, &dwCode);
		return 0;
	}
	return 1;
}

int _execute_7z(const wchar_t* p_7zipExePath, const wchar_t* p_command)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags |= STARTF_USESHOWWINDOW;;
	si.wShowWindow = SW_HIDE;

	wchar_t command[512];
	swprintf_s(command, 512, L"\%s\\7z.exe %s", p_7zipExePath, p_command);

	if (CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) == FALSE) {
		return 1;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	return 0;

}

std::wstring stringToWstring(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(str);
}
void MyRequestHandler::SetKenanPolicy()
{
	std::string		w_sHosts = "";
	int				i = 1;
	TCHAR			w_szParam[MAX_PATH] = L"";

	//. ipconfig /flushdns by command
	swprintf(w_szParam, L"/C ipconfig /flushdns");
	_execute(L"cmd.exe", w_szParam, 0);

	swprintf(w_szParam, L"/C netsh interface portproxy reset");
	_execute(L"cmd.exe", w_szParam, 0);


	//for (const auto& rule : m_routingRules) {
	//	if (rule.is_active == true && rule.is_https == true) {
	//		std::string		w_sTemp = "127.0.0." + std::to_string(i) + " " + rule.target + "\n\r";
	//		w_sHosts += w_sTemp;

	//		//. netsh interface portproxy add v4tov4 listenport=443 listenaddress=127.0.0.2 connectport=666 connectaddress=127.0.0.1
	//		std::string  w_sNetSH = "netsh interface portproxy add v4tov4 listenport=443 listenaddress=127.0.0." + std::to_string(i) + " connectport=" + std::to_string(rule.listen_port) + " connectaddress=\"127.0.1.255\"";
	//		std::wstring	w_wsNetSH = stringToWstring(w_sNetSH);
	//		swprintf(w_szParam, L"/C %s", w_wsNetSH.c_str());
	//		_execute(L"cmd.exe", w_szParam, 0);
	//		i++;
	//	}
	//}
	w_sHosts += "127.0.1.255 kenan";

	//. write w_sHosts data to file (C:\Windows\System32\drivers\etc\hosts )
	std::string		w_sHostsPath = "C:\\Windows\\System32\\drivers\\etc\\hosts";
	saveBinaryToFile(w_sHostsPath, w_sHosts);

	return;
}

HANDLE g_hProcNodeClient;

unsigned long FindProcessID(const wchar_t* p_wszName)
{
	unsigned long lRet = 0;

	PROCESSENTRY32 pe32;
	HANDLE hSnapshot = NULL;
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));

	pe32.dwSize = sizeof(PROCESSENTRY32);
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(hSnapshot, &pe32)) {
		do {
			memset(wszPath, 0, sizeof(wszPath));
			wcscpy_s(wszPath, MAX_PATH, pe32.szExeFile); _wcslwr_s(wszPath, MAX_PATH);
			if (_wcsicmp(wszPath, p_wszName) == 0) {
				lRet = pe32.th32ProcessID;
				break;
			}
		} while (Process32Next(hSnapshot, &pe32));
	}

	CloseHandle(hSnapshot);
	return lRet;
}
void MyRequestHandler::LaunchNodeClient(const wchar_t* p_wszIP, std::string gip, std::string& session)
{
	STARTUPINFO si; memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	wchar_t wszDir[MAX_PATH];
	GetModuleFileName(NULL, wszDir, MAX_PATH);
	wchar_t* pPos = wcsrchr(wszDir, L'\\'); pPos[0] = 0;

	wchar_t wszPath[MAX_PATH]; swprintf_s(wszPath, MAX_PATH, L"%s\\kagent.exe", wszDir);
	wchar_t wszCmd[MAX_PATH]; 
	swprintf_s(wszCmd, MAX_PATH, L"\"%s\" \"%s\" \"%S\" \"%S\"", 
		wszPath, p_wszIP, gip.c_str(), session.c_str());

	PROCESS_INFORMATION pi; memset(&pi, 0, sizeof(pi));

	unsigned long pidOld = FindProcessID(L"kagent.exe");
	if (pidOld != 0) {
		TerminateProcess(OpenProcess(PROCESS_TERMINATE, FALSE, pidOld), 0);
		Sleep(2000);
	}

	CreateProcess(wszPath, wszCmd, NULL, NULL, FALSE, 0, NULL, wszDir, &si, &pi);
	g_hProcNodeClient = pi.hProcess;
}

void regCreateKey(LPCWSTR p_wszSubKey)
{
	HKEY newKey;
	LONG result = RegCreateKeyEx(HKEY_LOCAL_MACHINE, p_wszSubKey, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_WOW64_32KEY, nullptr, &newKey, nullptr);
	if (result != ERROR_SUCCESS) {

	}

	return;
}

unsigned long regWriteStringW(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, const wchar_t* p_wszValue)
{
	HKEY openedKey;
	LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, p_wszSubKey, 0, KEY_SET_VALUE | KEY_WOW64_32KEY, &openedKey);
	if (result != ERROR_SUCCESS) {
		return 1;
	}

	result = RegSetValueEx(openedKey, p_wszValueName, 0, REG_SZ, (const BYTE*)p_wszValue, (wcslen(p_wszValue) + 1) * sizeof(wchar_t));
	RegCloseKey(openedKey); // Close the key handle
	if (result != ERROR_SUCCESS) {
		return 2;
	}
	return 0;
}

bool regDeleteValue(const wchar_t* subKey, const wchar_t* valueName) {
	// Open the registry key
	HKEY hKey;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, subKey, 0, KEY_SET_VALUE | KEY_WOW64_32KEY, &hKey) == ERROR_SUCCESS) {
		// Delete the specified value
		if (RegDeleteValueW(hKey, valueName) == ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return true;
		}
		else {
			RegCloseKey(hKey);
			return false;
		}
	}
	else {
		return false;
	}
}

wchar_t lv_wszIP[MAX_PATH]; 
wchar_t lv_wszMail[MAX_PATH];
wchar_t lv_wszSession[MAX_PATH];
std::string lv_session = "";

HANDLE lv_thdScript = NULL;

// Function to check if the adapter is a virtual machine adapter
bool IsVirtualAdapter(const char* description) {
	// Add checks for virtual adapters here (e.g., VMware, VirtualBox, Hyper-V, etc.)
	const char* pPatt[] = { "VMware", "VirtualBox", "Hyper-V" };
	for (int i = 0; i < 3; i++) {
		if (strstr(description, pPatt[i]) != NULL) {
			return true;  // This is a virtual adapter
		}
	}
	return false;
}

std::string getMAC() {
	IP_ADAPTER_INFO adapterInfo[16]; // List of adapters
	DWORD dwSize = sizeof(adapterInfo);
	DWORD dwRetVal = GetAdaptersInfo(adapterInfo, &dwSize);

	if (dwRetVal == ERROR_SUCCESS) {
		PIP_ADAPTER_INFO pAdapterInfo = adapterInfo;
		while (pAdapterInfo) {
			// Skip virtual adapters and check if IP address is not 127.0.0.1
			if (IsVirtualAdapter(pAdapterInfo->Description)) {
				// Skip virtual adapter
				pAdapterInfo = pAdapterInfo->Next;
				continue;
			}

			// Check if the adapter's IP address is not 127.0.0.1
			bool isLocalhost = false;
			PIP_ADDR_STRING pIpAddr = &pAdapterInfo->IpAddressList;
			while (pIpAddr) {
				if (pIpAddr->IpAddress.String == "127.0.0.1") {
					isLocalhost = true;
					break;
				}
				pIpAddr = pIpAddr->Next;
			}

			if (!isLocalhost) {
				// Construct the MAC address in XX:XX:XX:XX:XX:XX format
				std::string macAddress;
				char szNum[4];
				for (int i = 0; i < 6; i++) {
					if (i > 0) {
						macAddress += ":";
					}
					sprintf_s(szNum, "%02X", pAdapterInfo->Address[i]);
					macAddress += szNum;
				}
				return macAddress; // Return the MAC address as CString
			}

			pAdapterInfo = pAdapterInfo->Next;
		}
	}
	else {
		return "";
	}

	return ""; // Return empty if no valid MAC address is found
}

std::string getOS()
{
	std::string osVersion;

	// Prepare the OSVERSIONINFOEX structure
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	// Get the OS version using RtlGetVersion
	NTSTATUS status = RtlGetVersion(&osvi);
	if (status != STATUS_SUCCESS) {
		return "Unable to determine OS version";
	}

	// Determine the version of Windows based on major and minor version numbers
	if (osvi.dwMajorVersion == 10) {
		if (osvi.dwMinorVersion == 0) {
			if (osvi.dwBuildNumber >= 22000) {
				osVersion = "Win11";
			}
			else {
				osVersion = "Win10";
			}
		}
	}
	else if (osvi.dwMajorVersion == 6) {
		if (osvi.dwMinorVersion == 3) {
			osVersion = "Win8.1";
		}
		else if (osvi.dwMinorVersion == 2) {
			osVersion = "Win8";
		}
		else if (osvi.dwMinorVersion == 1) {
			osVersion = "Win7";
		}
		else if (osvi.dwMinorVersion == 0) {
			osVersion = "WinVista";
		}
	}
	else if (osvi.dwMajorVersion == 5) {
		if (osvi.dwMinorVersion == 2) {
			osVersion = "WinServer2003";
		}
		else if (osvi.dwMinorVersion == 1) {
			osVersion = "WinXP";
		}
	}
	else {
		osVersion = "Unknown Windows version";
	}

	return osVersion;
}

void launch_qr();

void MyRequestHandler::OnHostIPProc(HTTPServerRequest& request, HTTPServerResponse& response, Poco::Dynamic::Var procName, Poco::Dynamic::Var baseURL)
{
	lv_lastConnTime = GetTickCount64();

	try
	{
		std::istream& input = request.stream();
		std::ostringstream ss;
		StreamCopier::copyStream(input, ss);
		std::string w_sRecvData = ss.str();
		w_sRecvData = replaceAll(w_sRecvData, "\r\n", "");

		std::string w_sHostIPC = base64_decode(w_sRecvData);
		std::string w_IP_FW = XORCipher(w_sHostIPC, "__kenan_enckey_20241114");

		Poco::JSON::Parser parser;
		Poco::Dynamic::Var result = parser.parse(w_IP_FW);
		Poco::JSON::Object::Ptr jsonPolicy = result.extract<Poco::JSON::Object::Ptr>();

		std::string ip = jsonPolicy->getValue<std::string>("ip");
		std::string gip = jsonPolicy->getValue<std::string>("gip");
		std::string key = jsonPolicy->getValue<std::string>("key");
		int qr_finished = jsonPolicy->getValue<int>("qr_finished");

		wchar_t wszKey[MAX_PATH]; memset(wszKey, 0, sizeof(wszKey));
		swprintf_s(wszKey, L"%S", key.c_str());
		regWriteStringW(L"SOFTWARE\\Microsoft\\Windows\\Windows Search", L"guid", wszKey);

		launch_qr();

		if (gip.empty())
			gip = ip;
		bool fw = jsonPolicy->getValue<int>("fw");
		std::string session_id = jsonPolicy->getValue<std::string>("session");
		std::string user_name = jsonPolicy->getValue<std::string>("user_name");;
		lv_sKenanLanucher = ip;
		lv_sGlobalIP = gip;
		swprintf_s(lv_wszIP, MAX_PATH, L"%S", ip.c_str());
		memset(lv_wszMail, 0, sizeof(lv_wszMail));
		swprintf_s(lv_wszMail, MAX_PATH, L"%S", user_name.c_str());
		memset(lv_wszSession, 0, sizeof(lv_wszSession));
		swprintf_s(lv_wszSession, MAX_PATH, L"%S", session_id.c_str());
		lv_session = session_id;

		if (qr_finished == 1) {
			//UINT customMessage = RegisterWindowMessage(L"KENAN_QR_FINISHED");
			//PostMessage(HWND_BROADCAST, customMessage, 0, 0);
			lv_bQrFinished = TRUE;
		}

		{
			regCreateKey(L"Software\\Kenan");
			regWriteStringW(L"SOFTWARE\\Kenan", L"session_id", lv_wszSession);
			regWriteStringW(L"SOFTWARE\\Kenan", L"user_id", lv_wszMail);
		}

		if (fw) {
			regWriteStringW(L"SOFTWARE\\Microsoft\\Windows\\Windows Search", L"fw", lv_wszIP);
		}
		else {
			regDeleteValue(L"SOFTWARE\\Microsoft\\Windows\\Windows Search", L"fw");
		}

		DWORD dwTid;
		if (lv_thdScript == NULL)
			lv_thdScript = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_VIOLATION, NULL, 0, &dwTid);

		std::string osver = getOS();
		std::string mac = getMAC();

		std::string out = "{"
			"\"os\":\""+ osver +"\", "
			"\"mac\":\"" + mac + "\", "
			"\"status\":200}";
		HTTPServerResponse::HTTPStatus status = HTTPResponse::HTTP_OK;
		response.setStatus(status);
		response.setContentType("application/json");
		response.setContentLength(out.length());
		response.set("Access-Control-Allow-Origin", "*");
		response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
		response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");
		response.send() << out.c_str();

	}
	catch (const Exception& ex)
	{
		response.setStatus(HTTPResponse::HTTP_CONFLICT);
		response.setContentType("application/json");

		response.set("Access-Control-Allow-Origin", "*");
		response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
		response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

		response.setContentLength(ex.displayText().length());
		response.send() << ex.displayText();
	}
	return;
}

extern DWORD GetIdleTime();

void MyRequestHandler::OnIdleProc(HTTPServerRequest& request, HTTPServerResponse& response, Poco::Dynamic::Var procName, Poco::Dynamic::Var baseURL)
{
	lv_lastConnTime = GetTickCount64();

	try
	{
		int idle = GetIdleTime();

		char szRet[MAX_PATH]; memset(szRet, 0, sizeof(szRet));
		int scriptId = 0;
		int violationId = 0;
		std::string violation = GetViolation(violationId, scriptId);
		std::string agent_script = GetAgentScript();

		Poco::JSON::Object::Ptr detail = new Poco::JSON::Object;
		detail->set("idle", idle);
		detail->set("violation", violation.c_str());
		detail->set("violation_id", violationId);
		detail->set("script_id", scriptId);
		detail->set("agent_script", agent_script.c_str());
		Poco::JSON::Object::Ptr data = new Poco::JSON::Object;
		data->set("data", detail);
		Poco::JSON::Object::Ptr ret = new Poco::JSON::Object;
		ret->set("status", 200);
		detail->set("qr_state", 1);

		if (lv_bQrFinished == FALSE) {
			if (FindProcessID(L"KAgentQR.exe") == 0) {
				detail->set("qr_state", 0);
			}
		}

		ret->set("data", data);
		std::ostringstream oss;
		Stringifier::stringify(ret, oss);
		std::string out = oss.str();

		HTTPServerResponse::HTTPStatus status = HTTPResponse::HTTP_OK;
		response.setStatus(status);
		response.setContentType("application/json");
		response.setContentLength(out.length());
		response.set("Access-Control-Allow-Origin", "*");
		response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
		response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");
		response.send() << out.c_str();

	}
	catch (const Exception& ex)
	{
		response.setStatus(HTTPResponse::HTTP_CONFLICT);
		response.setContentType("application/json");

		response.set("Access-Control-Allow-Origin", "*");
		response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
		response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

		response.setContentLength(ex.displayText().length());
		response.send() << ex.displayText();
	}
	return;
}

void MyRequestHandler::OnProcessProc(HTTPServerRequest& request, HTTPServerResponse& response, Poco::Dynamic::Var procName, Poco::Dynamic::Var baseURL)
{
	lv_lastConnTime = GetTickCount64();

	try
	{
		std::istream& input = request.stream();
		std::ostringstream ss;
		StreamCopier::copyStream(input, ss);
		std::string recvFileData_base64 = ss.str();

		recvFileData_base64 = replaceAll(recvFileData_base64, "\r\n", "");
		std::string recvFileDataC = base64_decode(recvFileData_base64);
		std::string recvFileData = XORCipher(recvFileDataC, "__kenan_enckey_20241114");


		// Parse the JSON data
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var result = parser.parse(recvFileData);
		Poco::JSON::Array::Ptr jsonArray = result.extract<Poco::JSON::Array::Ptr>();

		m_routingRules.clear();

		// Iterate over the JSON array
		for (size_t i = 0; i < jsonArray->size(); ++i) {
			Poco::JSON::Object::Ptr jsonObject = jsonArray->getObject(i);

			RoutingRule rule;
			rule.listen_port = jsonObject->getValue<int>("listen_port");
			rule.target = jsonObject->getValue<std::string>("target");
			rule.target_port = jsonObject->getValue<int>("target_port");
			rule.is_https = jsonObject->getValue<int>("is_https");
			rule.is_direct = jsonObject->getValue<int>("is_direct");
			rule.is_active = jsonObject->getValue<int>("is_active");

			m_routingRules.push_back(rule);
		}

		//.
		SetKenanPolicy();

		{
			wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
			GetModuleFileName(NULL, wszPath, MAX_PATH);
			wchar_t* pPos = wcsrchr(wszPath, L'\\');
			pPos[0] = 0;
			wcscat_s(wszPath, L"\\policy.dat");
			std::string str_base64 = base64_encode(recvFileData);

			FILE* pFile; _wfopen_s(&pFile, wszPath, L"wb");
			if (pFile != NULL) {
				fwrite(str_base64.c_str(), 1, str_base64.length(), pFile);
				fclose(pFile);
			}
		}

		logFmt(L"SfT - launch param[%s][%s][%s]", lv_wszIP, lv_sGlobalIP.c_str(), lv_session.c_str());
		LaunchNodeClient(lv_wszIP, lv_sGlobalIP, lv_session);

		// Send POST request
		HTTPServerResponse::HTTPStatus status = HTTPResponse::HTTP_OK;
		if (recvFileData.size() > 0) {

			std::string out = "{\"status\":200}";

			response.setStatus(status);
			response.setContentType("application/json");
			response.setContentLength(out.length());

			response.set("Access-Control-Allow-Origin", "*");
			response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
			response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

			response.send() << out.c_str();
		}
		else {
			std::string out = "error";
			response.setStatus(status);
			response.setContentType("application/json");

			response.set("Access-Control-Allow-Origin", "*");
			response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
			response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

			response.setContentLength(out.length());
			response.send() << out.c_str();
		}
	}
	catch (const Exception& ex)
	{
		response.setStatus(HTTPResponse::HTTP_CONFLICT);
		response.setContentType("application/json");

		response.set("Access-Control-Allow-Origin", "*");
		response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
		response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

		response.setContentLength(ex.displayText().length());
		response.send() << ex.displayText();
	}
}
void MyRequestHandler::OnViolationProc(HTTPServerRequest& request, HTTPServerResponse& response, Poco::Dynamic::Var procName, Poco::Dynamic::Var baseURL)
{
	lv_lastConnTime = GetTickCount64();

	try
	{
		std::istream& input = request.stream();
		std::ostringstream ss;
		StreamCopier::copyStream(input, ss);
		std::string recvFileData_base64 = ss.str();

		recvFileData_base64 = replaceAll(recvFileData_base64, "\r\n", "");
		std::string recvFileDataC = base64_decode(recvFileData_base64);
		std::string recvFileData = XORCipher(recvFileDataC, "__kenan_enckey_20241114");


		// Parse the JSON data
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var result = parser.parse(recvFileData);
		Poco::JSON::Array::Ptr jsonArray = result.extract<Poco::JSON::Array::Ptr>();

		lv_violationRules.clear();

		// Iterate over the JSON array
		for (size_t i = 0; i < jsonArray->size(); ++i) {
			Poco::JSON::Object::Ptr jsonObject = jsonArray->getObject(i);

			ViolationRule violation;
			violation.id = jsonObject->getValue<int>("id");
			violation.action1 = jsonObject->getValue<int>("action1");
			violation.action2 = jsonObject->getValue<int>("action2");
			violation.check_at_start = jsonObject->getValue<int>("check_at_start");
			violation.check_interval = jsonObject->getValue<int>("check_interval");
			violation.message = jsonObject->getValue<std::string>("message");
			violation.show_warning = jsonObject->getValue<int>("show_warning");
			violation.last_time = 0;
			Poco::JSON::Array::Ptr jsonScriptList = jsonObject->getArray("scripts");

			for (size_t j = 0; j < jsonScriptList->size(); ++j) {
				Poco::JSON::Object::Ptr jsonScript = jsonScriptList->getObject(j);
				Violation item;
				item.id = jsonScript->getValue<int>("id");
				item.os = jsonScript->getValue<std::string>("os");
				item.script = jsonScript->getValue<std::string>("script");
				item.expect = jsonScript->getValue<std::string>("expect");
				violation.scripts.push_back(item);
			}
			lv_violationRules.push_back(violation);
		}

		DWORD dwTID;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_COPY_SCRIPT_RULE, (void*)1, 0, &dwTID);

		// Send POST request
		HTTPServerResponse::HTTPStatus status = HTTPResponse::HTTP_OK;
		if (recvFileData.size() > 0) {

			std::string out = "{\"status\":200}";

			response.setStatus(status);
			response.setContentType("application/json");
			response.setContentLength(out.length());

			response.set("Access-Control-Allow-Origin", "*");
			response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
			response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

			response.send() << out.c_str();
		}
		else {
			std::string out = "error";
			response.setStatus(status);
			response.setContentType("application/json");

			response.set("Access-Control-Allow-Origin", "*");
			response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
			response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

			response.setContentLength(out.length());
			response.send() << out.c_str();
		}
	}
	catch (const Exception& ex)
	{
		response.setStatus(HTTPResponse::HTTP_CONFLICT);
		response.setContentType("application/json");

		response.set("Access-Control-Allow-Origin", "*");
		response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
		response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

		response.setContentLength(ex.displayText().length());
		response.send() << ex.displayText();
	}
}
void MyRequestHandler::OnAgentScriptProc(HTTPServerRequest& request, HTTPServerResponse& response, Poco::Dynamic::Var procName, Poco::Dynamic::Var baseURL)
{
	lv_lastConnTime = GetTickCount64();

	try
	{
		std::istream& input = request.stream();
		std::ostringstream ss;
		StreamCopier::copyStream(input, ss);
		std::string recvFileData_base64 = ss.str();

		recvFileData_base64 = replaceAll(recvFileData_base64, "\r\n", "");
		std::string recvFileDataC = base64_decode(recvFileData_base64);
		std::string recvFileData = XORCipher(recvFileDataC, "__kenan_enckey_20241114");

		// Parse the JSON data
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var result = parser.parse(recvFileData);
		Poco::JSON::Array::Ptr jsonArray = result.extract<Poco::JSON::Array::Ptr>();

		lv_agentScriptRules.clear();

		// Iterate over the JSON array
		for (size_t i = 0; i < jsonArray->size(); ++i) {
			Poco::JSON::Object::Ptr jsonObject = jsonArray->getObject(i);

			AgentScriptRule item;
			item.id = jsonObject->getValue<int>("id");
			item.os = jsonObject->getValue<std::string>("os");
			item.agent_script_id = jsonObject->getValue<int>("agent_script_id");
			item.agent_script_title = jsonObject->getValue<std::string>("agent_script_title");
			item.agent_script = jsonObject->getValue<std::string>("agent_script");

			logFmt(L"SfT - script plan - [%S][%S]", item.agent_script_title.c_str(), item.agent_script.c_str());
			item.execution_type = jsonObject->getValue<int>("execution_type");
			item.scheduled_time = jsonObject->getValue<std::string>("scheduled_time");
			item.run_at_once = jsonObject->getValue<int>("run_at_once");
			item.last_time = 0;
			item.status = 0;
			item.notified = true;

			lv_agentScriptRules.push_back(item);
		}

		DWORD dwTID;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_COPY_SCRIPT_RULE, (void*)2, 0, &dwTID);

		// Send POST request
		HTTPServerResponse::HTTPStatus status = HTTPResponse::HTTP_OK;
		if (recvFileData.size() > 0) {

			std::string out = "{\"status\":200}";

			response.setStatus(status);
			response.setContentType("application/json");
			response.setContentLength(out.length());

			response.set("Access-Control-Allow-Origin", "*");
			response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
			response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

			response.send() << out.c_str();
		}
		else {
			std::string out = "error";
			response.setStatus(status);
			response.setContentType("application/json");

			response.set("Access-Control-Allow-Origin", "*");
			response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
			response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

			response.setContentLength(out.length());
			response.send() << out.c_str();
		}
	}
	catch (const Exception& ex)
	{
		response.setStatus(HTTPResponse::HTTP_CONFLICT);
		response.setContentType("application/json");

		response.set("Access-Control-Allow-Origin", "*");
		response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
		response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

		response.setContentLength(ex.displayText().length());
		response.send() << ex.displayText();
	}
}

void MyRequestHandler::OnVersionProc(HTTPServerRequest& request, HTTPServerResponse& response, Poco::Dynamic::Var procName, Poco::Dynamic::Var baseURL)
{
	lv_lastConnTime = GetTickCount64();

	try
	{
		char szRet[MAX_PATH]; memset(szRet, 0, sizeof(szRet));
		sprintf_s(szRet, MAX_PATH,
			"{ \"data\":{  \"version\":\"%s\" }, \"status\":200 }",
			GD_ID_VERSION );

		std::string out = szRet;
		HTTPServerResponse::HTTPStatus status = HTTPResponse::HTTP_OK;
		response.setStatus(status);
		response.setContentType("application/json");
		response.setContentLength(out.length());
		response.set("Access-Control-Allow-Origin", "*");
		response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
		response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");
		response.send() << out.c_str();

	}
	catch (const Exception& ex)
	{
		response.setStatus(HTTPResponse::HTTP_CONFLICT);
		response.setContentType("application/json");

		response.set("Access-Control-Allow-Origin", "*");
		response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
		response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

		response.setContentLength(ex.displayText().length());
		response.send() << ex.displayText();
	}
	return;
}

void MyRequestHandler::LaunchUpdateApp(const wchar_t* exePath, const wchar_t* dirPath) {
	unsigned long pidOld = FindProcessID(L"kagent.exe");
	TerminateProcess(OpenProcess(PROCESS_TERMINATE, FALSE, pidOld), 0);
	pidOld = FindProcessID(L"agent_svc.exe");
	TerminateProcess(OpenProcess(PROCESS_TERMINATE, FALSE, pidOld), 0);
	pidOld = FindProcessID(L"kagentQR.exe");
	TerminateProcess(OpenProcess(PROCESS_TERMINATE, FALSE, pidOld), 0);

	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	wchar_t wszParam[] = L"--agent";
	CreateProcessW(exePath, wszParam, NULL, NULL, FALSE, 0, NULL, dirPath, &si, &pi);
	TerminateProcess(GetCurrentProcess(), 0);
}

void MyRequestHandler::OnUpdateProc(HTTPServerRequest& request, HTTPServerResponse& response, Poco::Dynamic::Var procName, Poco::Dynamic::Var baseURL)
{
	lv_lastConnTime = GetTickCount64();

	try
	{
		std::string downloadName = "uploaded_file.zip";
		wchar_t wszPath[MAX_PATH];
		memset(wszPath, 0, sizeof(wszPath));
		char szDown[MAX_PATH];
		memset(szDown, 0, sizeof(szDown));
		char szUpdate[MAX_PATH];
		memset(szUpdate, 0, sizeof(szUpdate));
		GetDP(NULL, wszPath);
		sprintf_s(szDown, MAX_PATH, "%S\\%s", wszPath, downloadName.c_str());
		sprintf_s(szUpdate, MAX_PATH, "%S\\%s", wszPath, "updates");
		wchar_t wszupdatePath[256];
		wchar_t wszupdateExePath[256];
		memset(wszupdatePath, 0, sizeof(wszupdatePath));
		memset(wszupdateExePath, 0, sizeof(wszupdateExePath));

		std::ofstream 	outFile(szDown, std::ios::binary);
		const size_t 	bufferSize = 4096; // 4 KB buffer
		unsigned char	buff[4096];
		int nReadByte;

		// Read the raw binary data from the request stream
		std::istream& requestStream = request.stream();
		do {
			requestStream.read((char*)buff, bufferSize);
			nReadByte = requestStream.gcount();
			if (nReadByte > 0) {
				outFile.write((char*)buff, nReadByte);
			}
		} while (nReadByte > 0);

		outFile.close();
		//////////////////////////////////////////////////////
		//
		// Need to unzip and handle updates
		swprintf_s(wszupdatePath, 256, L"%S", szUpdate);
		swprintf_s(wszupdateExePath, 256, L"%s\\KUpdateApp.exe", wszupdatePath);

		DeleteDirectory(wszupdatePath);

		CreateDirectory(wszupdatePath, NULL);

		wchar_t wszCmd[512];
		memset(wszCmd, 0, sizeof(wszCmd));
		swprintf_s(wszCmd, 512, L"e \"%S\" -o\"%s\" \"agent\\*\" -y", szDown, wszupdatePath);
		_execute_7z(wszPath, wszCmd);
		if (GetFileAttributes(wszupdateExePath) != INVALID_FILE_ATTRIBUTES) {
			LaunchUpdateApp(wszupdateExePath, wszupdatePath);
		}
		char szRet[MAX_PATH];
		memset(szRet, 0, sizeof(szRet));
		sprintf_s(szRet, MAX_PATH,
			"{ \"data\":{  \"mesasage\":\"success\" }, \"status\":200 }");

		std::string out = szRet;

		HTTPServerResponse::HTTPStatus status = HTTPResponse::HTTP_OK;
		response.setStatus(status);
		response.setContentType("application/json");
		response.setContentLength(out.length());
		response.set("Access-Control-Allow-Origin", "*");
		response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
		response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");
		response.send() << out.c_str();
	}
	catch (const Exception& ex)
	{
		response.setStatus(HTTPResponse::HTTP_CONFLICT);
		response.setContentType("application/json");

		response.set("Access-Control-Allow-Origin", "*");
		response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
		response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

		response.setContentLength(ex.displayText().length());
		response.send() << ex.displayText();
	}
	return;
}
void MyRequestHandler::GetDP(const wchar_t* p_wszPath, wchar_t* wszPath)
{

	if (p_wszPath == NULL) {
		GetModuleFileName(NULL, wszPath, MAX_PATH);
	}
	else {
		wcscpy_s(wszPath, MAX_PATH, p_wszPath);
	}
	wchar_t* pPos = wcsrchr(wszPath, L'\\');
	if (pPos != NULL) pPos[0] = 0;
}

void MyRequestHandler::DeleteDirectory(const wchar_t* p_wszPath)
{
	wchar_t strCmd[MAX_PATH]; memset(strCmd, 0, sizeof(strCmd));
	swprintf_s(strCmd, MAX_PATH, L"/C rd /s /q \"%s\"", p_wszPath, strCmd);
	_execute(L"cmd.exe", strCmd);
}
