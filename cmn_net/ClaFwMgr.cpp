#include "pch.h"
#include "ClaFwMgr.h"
#include <stdio.h>
#include <iostream>

#include <netfw.h> // Include the Windows Firewall API header

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

bool ClaFwMgr::IsFirewallEnabled() {
    HRESULT hr;
    INetFwPolicy* pFwPolicy = NULL;
    INetFwProfile* pProfile = NULL;
    INetFwMgr* pFwMgr = NULL;

    // Initialize COM
    CoInitializeEx(0, COINIT_MULTITHREADED);
    CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

    // Create the firewall manager object
    hr = CoCreateInstance(__uuidof(NetFwMgr), NULL, CLSCTX_INPROC_SERVER,
        __uuidof(INetFwMgr), (void**)&pFwMgr);

    if (SUCCEEDED(hr)) {
        // Get the local policy
        hr = pFwMgr->get_LocalPolicy(&pFwPolicy);
        if (SUCCEEDED(hr)) {
            // Get the current profile
            hr = pFwPolicy->get_CurrentProfile(&pProfile);
            if (SUCCEEDED(hr)) {
                VARIANT_BOOL bEnabled;
                hr = pProfile->get_FirewallEnabled(&bEnabled);
                if (SUCCEEDED(hr)) {
                    pProfile->Release();
                    pFwPolicy->Release();
                    pFwMgr->Release();
                    CoUninitialize();
                    return (bEnabled == VARIANT_TRUE);
                }
                pProfile->Release();
            }
            pFwPolicy->Release();
        }
        pFwMgr->Release();
    }
    CoUninitialize();
    return false;
}

void ClaFwMgr::EnableFirewall() {
    HRESULT hr;
    INetFwPolicy2* pFwPolicy2 = NULL;
    // Initialize COM
    CoInitializeEx(0, COINIT_MULTITHREADED);
    CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

    VARIANT_BOOL sts;

    // Create the firewall policy object
    hr = CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER,
        __uuidof(INetFwPolicy2), (void**)&pFwPolicy2);

    if (SUCCEEDED(hr)) {
        // Enable the firewall for all profiles
        sts = VARIANT_FALSE;
        pFwPolicy2->get_FirewallEnabled(NET_FW_PROFILE2_DOMAIN, &sts);
        if (sts == VARIANT_FALSE)
        {
            hr = pFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_DOMAIN, VARIANT_TRUE);
        }

        sts = VARIANT_FALSE;
        pFwPolicy2->get_FirewallEnabled(NET_FW_PROFILE2_PRIVATE, &sts);
        if (sts == VARIANT_FALSE)
        {
            hr = pFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_PRIVATE, VARIANT_TRUE);
        }

        sts = VARIANT_FALSE;
        pFwPolicy2->get_FirewallEnabled(NET_FW_PROFILE2_PUBLIC, &sts);
        if (sts == VARIANT_FALSE)
        {
            hr = pFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_PUBLIC, VARIANT_TRUE);
        }

        pFwPolicy2->Release();
    }
    else {
        std::cout << "Failed to create Firewall Policy." << std::endl;
    }

    CoUninitialize();
}

int ClaFwMgr::AddAppRule(
    const wchar_t* p_wszName,
    const wchar_t* p_wszDesc,
    const wchar_t* p_wszApp,
    bool p_bIn
)
{
	HRESULT hr;

	// Initialize COM
	hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
	if (FAILED(hr)) {
		std::wcerr << L"Failed to initialize COM library.\n";
		return -1;
	}

	INetFwPolicy2* pNetFwPolicy2 = nullptr;

	// Create an instance of the firewall policy
	hr = CoCreateInstance(__uuidof(NetFwPolicy2), nullptr, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)&pNetFwPolicy2);
	if (FAILED(hr)) {
		std::wcerr << L"Failed to create firewall policy instance.\n";
		CoUninitialize();
		return -2;
	}

	// Create a new firewall rule
	INetFwRule* pFwRule = nullptr;
	hr = CoCreateInstance(__uuidof(NetFwRule), nullptr, CLSCTX_INPROC_SERVER, __uuidof(INetFwRule), (void**)&pFwRule);
	if (FAILED(hr)) {
		std::wcerr << L"Failed to create firewall rule instance.\n";
		pNetFwPolicy2->Release();
		CoUninitialize();
		return -3;
	}

	// Set the rule properties
	pFwRule->put_Name(_bstr_t(p_wszName));
    pFwRule->put_Description(_bstr_t(p_wszDesc));
	pFwRule->put_ApplicationName(_bstr_t(p_wszApp));
	pFwRule->put_Action(NET_FW_ACTION_ALLOW);        // Allow the connection
	pFwRule->put_Direction(NET_FW_RULE_DIR_MAX);      // Inbound rule
	pFwRule->put_Enabled(VARIANT_TRUE);              // Enable the rule

	// Add the rule to the firewall policy
	INetFwRules* pFwRules = nullptr;
	pNetFwPolicy2->get_Rules(&pFwRules);
	hr = pFwRules->Add(pFwRule);

	if (SUCCEEDED(hr)) {
		std::wcout << L"Inbound firewall rule added successfully.\n";
	}
	else {
		std::wcerr << L"Failed to add the firewall rule.\n";
	}

	// Clean up
	pFwRules->Release();
	pFwRule->Release();
	pNetFwPolicy2->Release();
	CoUninitialize();
    return 0;
}

int ClaFwMgr::AddPortRule(
    const wchar_t* p_wszName, 
    const wchar_t* p_wszDesc, 
    const std::string& p_strPort,
    bool p_bIn
)
{
    HRESULT hr;
    INetFwPolicy2* pNetFwPolicy2 = NULL;
    INetFwRule* pNetFwRule = NULL;
    INetFwRules* pNetFwRules = NULL;

    // Initialize COM
    CoInitializeEx(0, COINIT_MULTITHREADED);
    CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

    // Create the INetFwPolicy2 interface
    hr = CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pNetFwPolicy2));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create INetFwPolicy2 instance. HRESULT: " << hr << std::endl;
        return -1;
    }

    // Get the Rules collection
    hr = pNetFwPolicy2->get_Rules(&pNetFwRules);
    if (FAILED(hr))
    {
        std::cerr << "Failed to get the Rules collection. HRESULT: " << hr << std::endl;
        pNetFwPolicy2->Release();
        return -2;
    }

    // Create a new firewall rule
    hr = CoCreateInstance(__uuidof(NetFwRule), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pNetFwRule));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create INetFwRule instance. HRESULT: " << hr << std::endl;
        pNetFwRules->Release();
        pNetFwPolicy2->Release();
        return -3;
    }

    // Set rule properties
    pNetFwRule->put_Name(_bstr_t(p_wszName));
    pNetFwRule->put_Description(_bstr_t(p_wszDesc));
    pNetFwRule->put_Action(NET_FW_ACTION_BLOCK);
    pNetFwRule->put_Enabled(VARIANT_TRUE);
    if (p_bIn)
        pNetFwRule->put_Direction(NET_FW_RULE_DIR_IN);
    else
        pNetFwRule->put_Direction(NET_FW_RULE_DIR_OUT);

    // Add the port ranges to the rule
    pNetFwRule->put_Protocol(NET_FW_IP_PROTOCOL_TCP); // For TCP
    wchar_t wszPorts[MAX_PATH]; swprintf_s(wszPorts, MAX_PATH, L"%S", p_strPort.c_str());
    if (p_bIn)
        pNetFwRule->put_LocalPorts(_bstr_t(wszPorts)); // Block specified ports
    else
        pNetFwRule->put_RemotePorts(_bstr_t(wszPorts)); // Block specified ports

    // Add the rule to the firewall policy
    hr = pNetFwRules->Add(pNetFwRule);
    if (FAILED(hr))
    {
        std::cerr << "Failed to add the rule to the firewall policy. HRESULT: " << hr << std::endl;
    }
    else
    {
        std::cout << "Firewall rule added successfully." << std::endl;
    }

    // Release resources
    pNetFwRule->Release();
    pNetFwRules->Release();
    pNetFwPolicy2->Release();
    CoUninitialize();


    return 0;
}

bool ClaFwMgr::IsExistRule(const wchar_t* p_wszName, bool p_bIn)
{
    HRESULT hr;
    INetFwPolicy2* pNetFwPolicy2 = NULL;
    INetFwRules* pNetFwRules = NULL;
    INetFwRule* pNetFwRule = NULL;

    // Initialize COM
    CoInitializeEx(0, COINIT_MULTITHREADED);
    CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

    // Create the INetFwPolicy2 interface
    hr = CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pNetFwPolicy2));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create INetFwPolicy2 instance. HRESULT: " << hr << std::endl;
        return false;
    }

    // Get the Rules collection
    hr = pNetFwPolicy2->get_Rules(&pNetFwRules);
    if (FAILED(hr))
    {
        std::cerr << "Failed to get the Rules collection. HRESULT: " << hr << std::endl;
        pNetFwPolicy2->Release();
        return false;
    }

    // Check if the rule exists
    hr = pNetFwRules->Item(_bstr_t(p_wszName), &pNetFwRule);
    if (SUCCEEDED(hr))
    {
        BSTR remoteAddresses;
        pNetFwRule->get_RemoteAddresses(&remoteAddresses);
        // Rule exists
        pNetFwRule->Release();
        pNetFwRules->Release();
        pNetFwPolicy2->Release();
        CoUninitialize();
        return true;
    }

    // Rule does not exist
    pNetFwRules->Release();
    pNetFwPolicy2->Release();
    CoUninitialize();
    return false;
}

int ClaFwMgr::UpdatePortRule(const wchar_t* p_wszName, const wchar_t* p_wszDesc, const std::string& p_strPorts, bool p_bIn)
{
    HRESULT hr;
    INetFwPolicy2* pNetFwPolicy2 = NULL;
    INetFwRules* pNetFwRules = NULL;
    INetFwRule* pNetFwRule = NULL;

    // Initialize COM
    CoInitializeEx(0, COINIT_MULTITHREADED);
    CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

    // Create the INetFwPolicy2 interface
    hr = CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pNetFwPolicy2));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create INetFwPolicy2 instance. HRESULT: " << hr << std::endl;
        return -1;
    }

    // Get the Rules collection
    hr = pNetFwPolicy2->get_Rules(&pNetFwRules);
    if (FAILED(hr))
    {
        std::cerr << "Failed to get the Rules collection. HRESULT: " << hr << std::endl;
        pNetFwPolicy2->Release();
        return -2;
    }

    // Retrieve the existing rule
    hr = pNetFwRules->Item(_bstr_t(p_wszName), &pNetFwRule);
    if (FAILED(hr))
    {
        std::cerr << "Rule does not exist. HRESULT: " << hr << std::endl;
        pNetFwRules->Release();
        pNetFwPolicy2->Release();
        return -3;
    }

    // Update rule properties
    pNetFwRule->put_Description(_bstr_t(p_wszDesc));
    pNetFwRule->put_Enabled(VARIANT_TRUE);
    if (p_bIn)
        pNetFwRule->put_Direction(NET_FW_RULE_DIR_IN);
    else
        pNetFwRule->put_Direction(NET_FW_RULE_DIR_OUT);
    pNetFwRule->put_Action(NET_FW_ACTION_BLOCK);

    // Update the port ranges
    wchar_t wszPorts[MAX_PATH];
    swprintf_s(wszPorts, MAX_PATH, L"%S", p_strPorts.c_str());
    if (p_bIn)
        pNetFwRule->put_LocalPorts(_bstr_t(wszPorts)); // Update specified ports
    else
        pNetFwRule->put_RemotePorts(_bstr_t(wszPorts)); // Update specified ports

    // Release resources
    pNetFwRule->Release();
    pNetFwRules->Release();
    pNetFwPolicy2->Release();
    CoUninitialize();

    return 0;
}

bool ClaFwMgr::DeleteRule(const wchar_t* p_wszName, bool p_bInbound)
{
    HRESULT hr;
    INetFwPolicy2* pNetFwPolicy2 = NULL;
    INetFwRules* pNetFwRules = NULL;

    // Initialize COM
    CoInitializeEx(0, COINIT_MULTITHREADED);
    CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

    // Create the INetFwPolicy2 interface
    hr = CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pNetFwPolicy2));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create INetFwPolicy2 instance. HRESULT: " << hr << std::endl;
        return false;
    }

    // Get the Rules collection
    hr = pNetFwPolicy2->get_Rules(&pNetFwRules);
    if (FAILED(hr))
    {
        std::cerr << "Failed to get the Rules collection. HRESULT: " << hr << std::endl;
        pNetFwPolicy2->Release();
        return false;
    }

    // Check if the rule exists
    INetFwRule* pNetFwRule = NULL;
    hr = pNetFwRules->Item(_bstr_t(p_wszName), &pNetFwRule);
    if (SUCCEEDED(hr))
    {
        // Check if the rule direction matches
        NET_FW_RULE_DIRECTION direction;
        hr = pNetFwRule->get_Direction(&direction);
        if (SUCCEEDED(hr))
        {
            // Attempt to delete the rule
            hr = pNetFwRules->Remove(_bstr_t(p_wszName));
            if (FAILED(hr))
            {
                std::cerr << "Failed to delete the rule. HRESULT: " << hr << std::endl;
                pNetFwRule->Release();
                pNetFwRules->Release();
                pNetFwPolicy2->Release();
                CoUninitialize();
                return false;
            }
        }
        else
        {
            std::cerr << "Rule direction does not match. Cannot delete the rule." << std::endl;
            pNetFwRule->Release();
            pNetFwRules->Release();
            pNetFwPolicy2->Release();
            CoUninitialize();
            return false;
        }

        pNetFwRule->Release();
    }
    else
    {
        std::cerr << "Rule not found. HRESULT: " << hr << std::endl;
    }

    // Clean up
    pNetFwRules->Release();
    pNetFwPolicy2->Release();
    CoUninitialize();

    return true;
}

int ClaFwMgr::AddRemoteIpRule(
    const wchar_t* p_wszName,
    const wchar_t* p_wszDesc,
    const std::string& p_strIPs, // New parameter for remote IPs
    bool p_bIn
)
{
    HRESULT hr;
    INetFwPolicy2* pNetFwPolicy2 = NULL;
    INetFwRule* pNetFwRule = NULL;
    INetFwRules* pNetFwRules = NULL;

    // Initialize COM
    CoInitializeEx(0, COINIT_MULTITHREADED);
    CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

    // Create the INetFwPolicy2 interface
    hr = CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pNetFwPolicy2));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create INetFwPolicy2 instance. HRESULT: " << hr << std::endl;
        return -1;
    }

    // Get the Rules collection
    hr = pNetFwPolicy2->get_Rules(&pNetFwRules);
    if (FAILED(hr))
    {
        std::cerr << "Failed to get the Rules collection. HRESULT: " << hr << std::endl;
        pNetFwPolicy2->Release();
        return -2;
    }

    // Create a new firewall rule
    hr = CoCreateInstance(__uuidof(NetFwRule), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pNetFwRule));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create INetFwRule instance. HRESULT: " << hr << std::endl;
        pNetFwRules->Release();
        pNetFwPolicy2->Release();
        return -3;
    }

    // Set rule properties
    pNetFwRule->put_Name(_bstr_t(p_wszName));
    pNetFwRule->put_Description(_bstr_t(p_wszDesc));
    pNetFwRule->put_Action(NET_FW_ACTION_ALLOW); // Change to ALLOW if needed
    pNetFwRule->put_Enabled(VARIANT_TRUE);
    pNetFwRule->put_Direction(p_bIn ? NET_FW_RULE_DIR_IN : NET_FW_RULE_DIR_OUT);

    // Add the port ranges to the rule
    pNetFwRule->put_Protocol(NET_FW_IP_PROTOCOL_TCP); // For TCP
    wchar_t wszIPs[MAX_PATH];
    swprintf_s(wszIPs, MAX_PATH, L"%S", p_strIPs.c_str());
    pNetFwRule->put_RemoteAddresses(_bstr_t(wszIPs));
    pNetFwRule->put_Action(NET_FW_ACTION_BLOCK);

    // Add the rule to the firewall policy
    hr = pNetFwRules->Add(pNetFwRule);
    if (FAILED(hr))
    {
        std::cerr << "Failed to add the rule to the firewall policy. HRESULT: " << hr << std::endl;
    }
    else
    {
        std::cout << "Firewall rule added successfully." << std::endl;
    }

    // Release resources
    pNetFwRule->Release();
    pNetFwRules->Release();
    pNetFwPolicy2->Release();
    CoUninitialize();

    return 0;
}

int ClaFwMgr::UpdateRemoteIpRule(
    const wchar_t* p_wszName, 
    const wchar_t* p_wszDesc, 
    const std::string& p_strIPs, 
    bool p_bIn)
{
    HRESULT hr;
    INetFwPolicy2* pNetFwPolicy2 = NULL;
    INetFwRules* pNetFwRules = NULL;
    INetFwRule* pNetFwRule = NULL;

    // Initialize COM
    CoInitializeEx(0, COINIT_MULTITHREADED);
    CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

    // Create the INetFwPolicy2 interface
    hr = CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pNetFwPolicy2));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create INetFwPolicy2 instance. HRESULT: " << hr << std::endl;
        return -1;
    }

    // Get the Rules collection
    hr = pNetFwPolicy2->get_Rules(&pNetFwRules);
    if (FAILED(hr))
    {
        std::cerr << "Failed to get the Rules collection. HRESULT: " << hr << std::endl;
        pNetFwPolicy2->Release();
        return -2;
    }

    // Retrieve the existing rule
    hr = pNetFwRules->Item(_bstr_t(p_wszName), &pNetFwRule);
    if (FAILED(hr))
    {
        std::cerr << "Rule does not exist. HRESULT: " << hr << std::endl;
        pNetFwRules->Release();
        pNetFwPolicy2->Release();
        return -3;
    }

    // Update rule properties
//    pNetFwRule->put_Name(_bstr_t(p_wszName));
    pNetFwRule->put_Description(_bstr_t(p_wszDesc));
    pNetFwRule->put_Action(NET_FW_ACTION_ALLOW); // Change to ALLOW if needed
    pNetFwRule->put_Enabled(VARIANT_TRUE);
    pNetFwRule->put_Direction(p_bIn ? NET_FW_RULE_DIR_IN : NET_FW_RULE_DIR_OUT);

    // Update rule properties
    pNetFwRule->put_Protocol(NET_FW_IP_PROTOCOL_TCP); // For TCP
    wchar_t wszIPs[MAX_PATH];
    swprintf_s(wszIPs, MAX_PATH, L"%S", p_strIPs.c_str());
    pNetFwRule->put_RemoteAddresses(_bstr_t(wszIPs));
    pNetFwRule->put_Action(NET_FW_ACTION_BLOCK);

    // Release resources
    pNetFwRule->Release();
    pNetFwRules->Release();
    pNetFwPolicy2->Release();
    CoUninitialize();

    return 0;
}
