#include "pch.h"
#include "ClaNetAdapter.h"

#include <iphlpapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "iphlpapi.lib") // Link with iphlpapi.lib
#pragma comment(lib, "ws2_32.lib")   // Link with Winsock library

// Function to check if the adapter is a virtual machine adapter
bool IsVirtualAdapter(const char* description) {
    // Add checks for virtual adapters here (e.g., VMware, VirtualBox, Hyper-V, etc.)
    char* pPatt[] = { "VMware", "VirtualBox", "Hyper-V" };
    for (int i = 0; i < 3; i++) {
        if (strstr(description, pPatt[i]) != NULL) {
            return true;  // This is a virtual adapter
        }
    }
    return false;
}


CString ClaNetAdapter::GetIP()
{
    CString strRet;

    // Allocate memory for the adapter addresses
    ULONG outBufLen = 50000;
    IP_ADAPTER_ADDRESSES* pAddresses = NULL;

    // Get the adapter addresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    ULONG family = AF_INET; // IPv4
    ULONG outBufLenRequired;

    outBufLen = 0;
    pAddresses = NULL;
    GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);
    pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);

    if (GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen) == NO_ERROR) {
        IP_ADAPTER_ADDRESSES* pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            char szDesc[200]; sprintf_s(szDesc, 200, "%S", pCurrAddresses->Description);
            if (IsVirtualAdapter(szDesc)) {
                pCurrAddresses = pCurrAddresses->Next;
                continue;
            }

            // Check if the adapter is up and not a loopback
            if (pCurrAddresses->OperStatus == IfOperStatusUp &&
                !(pCurrAddresses->Flags & IP_ADAPTER_NO_MULTICAST)) {

                // Loop through the IP addresses
                IP_ADAPTER_UNICAST_ADDRESS* pUnicast = pCurrAddresses->FirstUnicastAddress;
                while (pUnicast) {
                    // Check for IPv4 addresses
                    if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
                        struct sockaddr_in* sa_in = (struct sockaddr_in*)pUnicast->Address.lpSockaddr;
                        TCHAR ipstr[INET_ADDRSTRLEN];
                        InetNtop(AF_INET, &sa_in->sin_addr, ipstr, sizeof(ipstr) / sizeof(TCHAR));
                        strRet = ipstr;
                    }
                    pUnicast = pUnicast->Next;
                    if (strRet == L"127.0.0.1" || strRet.IsEmpty()) continue;
                    break;
                }
            }
            pCurrAddresses = pCurrAddresses->Next;
            if (strRet == L"127.0.0.1" || strRet.IsEmpty()) continue;
            break;
        }
    }
    else {
//        AfxMessageBox(_T("GetAdaptersAddresses failed"));
    }

    // Clean up
    if (pAddresses) free(pAddresses);
    return strRet; // Exit the application
}

CString ClaNetAdapter::GetIPs()
{
    CString strRet;
    CString strIP;

    // Allocate memory for the adapter addresses
    ULONG outBufLen = 50000;
    IP_ADAPTER_ADDRESSES* pAddresses = NULL;

    // Get the adapter addresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    ULONG family = AF_INET; // IPv4
    ULONG outBufLenRequired;

    outBufLen = 0;
    pAddresses = NULL;
    GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);
    pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);

    if (GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen) == NO_ERROR) {
        IP_ADAPTER_ADDRESSES* pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            char szDesc[200]; sprintf_s(szDesc, 200, "%S", pCurrAddresses->Description);
            if (IsVirtualAdapter(szDesc)) {
                pCurrAddresses = pCurrAddresses->Next;
                continue;
            }

            // Check if the adapter is up and not a loopback
            if (pCurrAddresses->OperStatus == IfOperStatusUp &&
                !(pCurrAddresses->Flags & IP_ADAPTER_NO_MULTICAST)) {

                // Loop through the IP addresses
                IP_ADAPTER_UNICAST_ADDRESS* pUnicast = pCurrAddresses->FirstUnicastAddress;
                while (pUnicast) {
                    // Check for IPv4 addresses
                    if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
                        struct sockaddr_in* sa_in = (struct sockaddr_in*)pUnicast->Address.lpSockaddr;
                        TCHAR ipstr[INET_ADDRSTRLEN];
                        InetNtop(AF_INET, &sa_in->sin_addr, ipstr, sizeof(ipstr) / sizeof(TCHAR));
                        strIP = ipstr;
                    }
                    pUnicast = pUnicast->Next;
                    if (strIP == L"127.0.0.1" || strIP.IsEmpty()) continue;
//                    break;
                }
            }
            pCurrAddresses = pCurrAddresses->Next;
            if (strIP == L"127.0.0.1" || strIP.IsEmpty()) continue;


            strRet = strIP + L"|";
        }
    }
    else {
        //        AfxMessageBox(_T("GetAdaptersAddresses failed"));
    }

    // Clean up
    if (pAddresses) free(pAddresses);
    return strRet; // Exit the application
}

// Function to get the MAC address in the format XX:XX:XX:XX:XX:XX as CString
CString ClaNetAdapter::GetMAC() {
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
                CString macAddress;
                for (int i = 0; i < 6; i++) {
                    if (i > 0) {
                        macAddress += ":";
                    }
                    macAddress.AppendFormat(L"%02X", pAdapterInfo->Address[i]);
                }
                return macAddress; // Return the MAC address as CString
            }

            pAdapterInfo = pAdapterInfo->Next;
        }
    }
    else {
        return L"";
    }

    return L""; // Return empty if no valid MAC address is found
}


