
//#include "pch.h"
#include "MiKeyMgr.h"
#include <shlobj.h>
#include <string.h>
#include <stdio.h>
#include <ntddscsi.h>
#include <winioctl.h>

#define LD_MARK_REQUEST		'milr'
#define LD_MARK_RESPONSE	'mils'

void getUniqueID(unsigned char* p_pID) {
	HANDLE hDevice;
	//
	// get harddisk0 serial number
	//
	hDevice = CreateFile(L"\\\\.\\PhysicalDrive0",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE) {
		//		std::cerr << "Failed to open disk. Error code: " << GetLastError() << std::endl;
		return;
	}

	STORAGE_PROPERTY_QUERY query;
	query.PropertyId = StorageDeviceProperty;
	query.QueryType = PropertyStandardQuery;

	STORAGE_DESCRIPTOR_HEADER header;
	DWORD dwBytesReturned = 0;

	if (!DeviceIoControl(hDevice,
		IOCTL_STORAGE_QUERY_PROPERTY,
		&query,
		sizeof(STORAGE_PROPERTY_QUERY),
		&header,
		sizeof(STORAGE_DESCRIPTOR_HEADER),
		&dwBytesReturned,
		NULL)) {
		//		std::cerr << "DeviceIoControl failed. Error code: " << GetLastError() << std::endl;
		CloseHandle(hDevice);
		return;
	}

	DWORD dwOutBufferSize = sizeof(STORAGE_DEVICE_DESCRIPTOR) + header.Size;
	auto* pDevDesc = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(malloc(dwOutBufferSize));

	if (!DeviceIoControl(hDevice,
		IOCTL_STORAGE_QUERY_PROPERTY,
		&query,
		sizeof(STORAGE_PROPERTY_QUERY),
		pDevDesc,
		dwOutBufferSize,
		&dwBytesReturned,
		NULL)) {
		//		std::cerr << "DeviceIoControl failed. Error code: " << GetLastError() << std::endl;
		CloseHandle(hDevice);
		free(pDevDesc);
		return;
	}

	// Serial number starts from the offset specified in the descriptor
	char* serialNumber = reinterpret_cast<char*>(pDevDesc) + pDevDesc->SerialNumberOffset;
	char szT[32]; memset(szT, 0, sizeof(szT));
	strncpy_s(szT, 32, serialNumber, 32);
	for (int i = strlen(szT); i < 32; i++) {
		szT[i] = 0;
	}

	GUID c =
	{ 0x8873d757, 0xf26b, 0x4e7a, { 0x91, 0xe3, 0x2b, 0x6f, 0x4f, 0x55, 0xfb, 0x74 } };

	unsigned char* pp = (unsigned char*)&c;
	for (int i = 0; i < 16; i++) {
		p_pID[i] = pp[i] ^ szT[i];
	}
	for (int i = 0; i < 16; i++) {
		p_pID[i + 16] = pp[i] ^ szT[i + 16];
	}
}

bool _is_file_exist(const char* p_szPath) {
	if (GetFileAttributesA(p_szPath) == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return true;
}

void mil_get_request_path(char* p_szPath, int product) {
	char szPath[MAX_PATH]; memset(szPath, 0, sizeof(szPath));
	HRESULT hr = SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath);

	sprintf_s(p_szPath, MAX_PATH, "%s\\mireq%d.miq", szPath, product);
}

void mil_get_response_path(char* p_szPath, int product) {
	char szPath[MAX_PATH]; memset(szPath, 0, sizeof(szPath));
	HRESULT hr = SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath);

	sprintf_s(p_szPath, MAX_PATH, "%s\\mirsp%d.miq", szPath, product);
}

void _fill_random(void* p_pstData, int p_nSize) {
	unsigned char* p = (unsigned char*)p_pstData;

	for (int i = 0; i < p_nSize; i++) {
		p[i] = (rand() * rand()) & 0xFF;
	}

}

void _create_struct(ST_REQUEST* p_pstRequest) {

	_fill_random(p_pstRequest, sizeof(ST_REQUEST));

	getUniqueID(p_pstRequest->m_pUniqueID);

	p_pstRequest->m_lMark = LD_MARK_REQUEST;
}

void _create_struct(ST_RESPONSE* p_pstRsp) {

	_fill_random(p_pstRsp, sizeof(ST_RESPONSE));

	p_pstRsp->m_lMark = LD_MARK_REQUEST;
}

void _put_hash(ST_REQUEST* p_pstRequest) {
	unsigned char* p = (unsigned char*)p_pstRequest;
	unsigned char szHash[32]; memset(szHash, 0, sizeof(szHash));
	for (int i = 0; i < sizeof(ST_REQUEST) - 32; i += 32) {
		for (int j = 0; j < 32; j++) {
			szHash[j] = szHash[j] ^ p[i + j];
		}
	}
	memcpy(p_pstRequest->m_pHash, szHash, 32);
}

bool _check_hash(ST_REQUEST* p_pstRequest) {
	unsigned char* p = (unsigned char*)p_pstRequest;
	unsigned char szHash[32]; memset(szHash, 0, sizeof(szHash));
	for (int i = 0; i < sizeof(ST_REQUEST) - 32; i += 32) {
		for (int j = 0; j < 32; j++) {
			szHash[j] = szHash[j] ^ p[i + j];
		}
	}

	return memcmp(p_pstRequest->m_pHash, szHash, 32) == 0;
}

bool _check_hash(ST_RESPONSE* p_pstResponse) {
	unsigned char* p = (unsigned char*)p_pstResponse;
	unsigned char szHash[32]; memset(szHash, 0, sizeof(szHash));
	for (int i = 0; i < sizeof(ST_RESPONSE) - 32; i += 32) {
		for (int j = 0; j < 32; j++) {
			szHash[j] = szHash[j] ^ p[i + j];
		}
	}

	return memcmp(p_pstResponse->m_pHash, szHash, 32) == 0;
}


void _enc_dec(void* p_pBuff, int nSize) {
	unsigned char* pBuff = (unsigned char*)p_pBuff;
	// {22DE2144-7DD8-41E2-B924-4DE6FE8216C2}
	GUID guidKey = { 0x22de2144, 0x7dd8, 0x41e2, { 0xb9, 0x24, 0x4d, 0xe6, 0xfe, 0x82, 0x16, 0xc2 } };
	unsigned char* pKey = (unsigned char*)&guidKey;
	for (int i = 0; i < nSize; i += 16) {
		for (int j = 0; j < 16; j++) {
			pBuff[i + j] = pBuff[i + j] ^ pKey[j];
		}
	}
}

void _enc_dec(void* p_pBuff, int nSize, void* p_pUniqID) {
	unsigned char* pBuff = (unsigned char*)p_pBuff;
	unsigned char* pHash = (unsigned char*)p_pUniqID;

	for (int i = 0; i < nSize; i += 32) {
		for (int j = 0; j < 32; j++) {
			pBuff[i + j] = pBuff[i + j] ^ pHash[j];
		}
	}
}

void mil_create_request(int p_nProduct, int p_nOS, int p_nDay)
{
	ST_REQUEST stRequest;
	_create_struct(&stRequest);

	stRequest.m_nProduct = p_nProduct;
	stRequest.m_nOS = p_nOS;
	stRequest.m_nRequestDays = p_nDay;

	_put_hash(&stRequest);

	char szPath[MAX_PATH]; memset(szPath, 0, sizeof(szPath));
	mil_get_request_path(szPath, p_nProduct);

	_enc_dec(&stRequest, sizeof(stRequest));

	FILE* pFile; fopen_s(&pFile, szPath, "wb");
	if (pFile == NULL) {
		return;
	}
	fwrite(&stRequest, 1, sizeof(stRequest), pFile);
	fclose(pFile);
}

unsigned int mil_read_request(const char* p_szPath, ST_REQUEST* p_pstRequest)
{
	FILE* pFile; fopen_s(&pFile, p_szPath, "rb");
	if (pFile == NULL) {
		return 1;
	}

	ST_REQUEST stRequest; memset(&stRequest, 0, sizeof(stRequest));
	int lRead = fread(&stRequest, 1, sizeof(stRequest), pFile);
	if (lRead != sizeof(stRequest)) {
		fclose(pFile); return 2;
	}
	fclose(pFile);

	_enc_dec(&stRequest, sizeof(stRequest));

	if (_check_hash(&stRequest) == false) {
		return 3;
	}

	if (stRequest.m_lMark != LD_MARK_REQUEST) {
		return 4;
	}

	memcpy(p_pstRequest, &stRequest, sizeof(stRequest));
	return 0;
}

void mil_get_product(int p_nProduct, char* p_szProduct)
{
	if (p_nProduct == GD_PRODUCT_DR) {
		strcpy_s(p_szProduct, 64, "Document Reader");
	}
	else if (p_nProduct == GD_PRODUCT_FACE) {
		strcpy_s(p_szProduct, 64, "Face SDK");
	}
	else {
		strcpy_s(p_szProduct, 64, "Unknown");
	}
}

void mil_get_os(int p_nOS, char* p_szOS)
{
	if (p_nOS == GD_OS_ANDROID) {
		strcpy_s(p_szOS, 64, "Android");
	}
	else if (p_nOS == GD_OS_LINUX) {
		strcpy_s(p_szOS, 64, "Linux");
	}
	else if (p_nOS == GD_OS_WINDOW) {
		strcpy_s(p_szOS, 64, "Windows");
	}
	else {
		strcpy_s(p_szOS, 64, "Unknown");
	}
}

int mil_read_response(const char* p_szPath, ST_RESPONSE* p_pstRsp, unsigned char* p_pUniqID)
{
	FILE* pFile; fopen_s(&pFile, p_szPath, "rb");
	if (pFile == NULL) {
		return 1;
	}

	ST_RESPONSE* pstResponse = (ST_RESPONSE*)malloc(sizeof(ST_RESPONSE));

	int lRead = fread(pstResponse, 1, sizeof(ST_RESPONSE), pFile);
	if (lRead != sizeof(ST_RESPONSE)) {
		free(pstResponse);
		fclose(pFile); return 2;
	}
	fclose(pFile);

	_enc_dec(pstResponse, sizeof(ST_RESPONSE), p_pUniqID);

	if (_check_hash(pstResponse) == false) {
		return 3;
	}

	if (pstResponse->m_lMark != LD_MARK_REQUEST) {
		return 4;
	}

	memcpy(p_pstRsp, pstResponse, sizeof(ST_RESPONSE));
	return 0;
}

__int64 mil_read_license(ST_RESPONSE* p_pstRsp)
{
	ST_REQUEST stReq;
	char szPath[MAX_PATH]; memset(szPath, 0, sizeof(szPath));
	unsigned char pUnique[32];

	getUniqueID(pUnique);

	memset(szPath, 0, sizeof(szPath));
	mil_get_response_path(szPath, GD_PRODUCT_FACE);
	if (_is_file_exist(szPath) == false) {
		return -1;
	}

	ST_RESPONSE* pstRsp = (ST_RESPONSE*)malloc(sizeof(ST_RESPONSE));
	if (mil_read_response(szPath, pstRsp, pUnique) != 0) {
		free(pstRsp);
		return 0;
	}

	if (p_pstRsp)
		memcpy(p_pstRsp, pstRsp, sizeof(ST_RESPONSE));
	free(pstRsp);
	return p_pstRsp->m_lExpire;
}

#ifdef __MI_SERVER__

void _put_hash(ST_RESPONSE* p_pstResponse) {
	unsigned char* p = (unsigned char*)p_pstResponse;
	unsigned char szHash[32]; memset(szHash, 0, sizeof(szHash));
	for (int i = 0; i < sizeof(ST_RESPONSE) - 32; i += 32) {
		for (int j = 0; j < 32; j++) {
			szHash[j] = szHash[j] ^ p[i + j];
		}
	}
	memcpy(p_pstResponse->m_pHash, szHash, 32);
}

int _create_keyfile_dr(
	ST_RESPONSE* p_pstRsp,
	int p_nOS,
	__int64 p_nExpire,
	unsigned char* p_pUniqID)
{
	_create_struct(p_pstRsp);

	p_pstRsp->m_nProduct = GD_PRODUCT_FACE;
	p_pstRsp->m_nOS = p_nOS;
	p_pstRsp->m_lExpire = p_nExpire;

	return 0;
}


int mil_create_response(
	const wchar_t* p_wszPath,
	int p_nProuct, int p_nOS,
	__int64 p_nExpire,
	unsigned char* p_pUniqID)
{
	int nRet = 0;
	ST_RESPONSE* pstRsp = (ST_RESPONSE*)malloc(sizeof(ST_RESPONSE));

	if (p_nProuct == GD_PRODUCT_FACE) {
		nRet = _create_keyfile_dr(pstRsp, p_nOS, p_nExpire, p_pUniqID);
	}

	if (nRet != 0) {
		goto L_EXIT;
	}

	{
		_put_hash(pstRsp);
		_enc_dec(pstRsp, sizeof(ST_RESPONSE), p_pUniqID);
	}

	{
		FILE* pFile; _wfopen_s(&pFile, p_wszPath, L"wb");
		if (pFile == NULL) {
			nRet = 2;
			goto L_EXIT;
		}

		fwrite(pstRsp, 1, sizeof(ST_RESPONSE), pFile);
		fclose(pFile);
		nRet = 0;
	}

L_EXIT:
	free(pstRsp);
	return nRet;
}

#endif//__MI_SERVER__