#pragma once

#include <string>

class ClaFwMgr
{
public:
	ClaFwMgr() {};
	~ClaFwMgr() {};

public:
	static bool IsFirewallEnabled();
	static void EnableFirewall();
	static int AddAppRule(
		const wchar_t* p_wszName,
		const wchar_t* p_wszDesc,
		const wchar_t* p_wszApp,
		bool p_bIn
	);

	static int AddPortRule(
		const wchar_t* p_wszName,
		const wchar_t* p_wszDesc,
		const std::string& p_strPort,
		bool p_bIn
	);
	static bool IsExistRule(const wchar_t* p_wszName, bool p_bIn);
	static int UpdatePortRule(
		const wchar_t* p_wszName, 
		const wchar_t* p_wszDesc, 
		const std::string& p_strPorts, 
		bool p_bIn);
	static int AddRemoteIpRule(const wchar_t* p_wszName,
		const wchar_t* p_wszDesc,
		const std::string& p_vecRemoteIPs, // New parameter for remote IPs
		bool p_bIn);
	static int UpdateRemoteIpRule(const wchar_t* p_wszName, 
		const wchar_t* p_wszDesc, 
		const std::string& p_strIPs, 
		bool p_bIn);
	static bool DeleteRule(const wchar_t* p_wszName, bool p_bIn);
};

