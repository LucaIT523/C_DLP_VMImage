#pragma once

#define GD_PRODUCT_FACE		1
#define GD_PRODUCT_DR		2

#define GD_OS_ANDROID		1
#define GD_OS_WINDOW		2
#define GD_OS_LINUX			3

typedef struct tagRequest {
	union {
		struct {
			unsigned long m_lMark;
			unsigned int m_nProduct;	//	1 : Face, 2 : Document reader
			unsigned int m_nOS;			//	1 : Android, 2 : Windows, 3 : Linux
			unsigned int m_nRequestDays;//	a number of days requested.
			unsigned char m_pUniqueID[32];
		};

		unsigned char m_pBuff[512];
	};

	unsigned char m_pHash[32]; // this time, it will be use only 4 bytes, for crc
}ST_REQUEST;

void mil_get_request_path(char* p_szPath, int product);
void mil_get_response_path(char* p_szPath, int product);
void mil_create_request(int p_nProduct, int p_nOS, int p_nDay);
unsigned int mil_read_request(const char* p_szPath, ST_REQUEST* p_pstRequest);

void mil_get_product(int p_nProduct, char* p_szProduct);
void mil_get_os(int p_nOS, char* p_szOS);

typedef struct tagResponse {
	union {
		struct {
			unsigned long m_lMark;
			unsigned int m_nProduct;	//	1 : Face, 2 : Document reader
			unsigned int m_nOS;			//	1 : Android, 2 : Windows, 3 : Linux
			__int64 m_lExpire;//	a number of days requested.
		};

		unsigned char m_pBuffHeader[64];
	};
	union {
		struct {
			char m_szDrInit[0x500];
			char m_szDrTrans1[0x500];
			char m_szDrTrans2[0x500];
		};

		unsigned char m_pBuff[0x1600];
	};
	unsigned char m_pHash[32];
}ST_RESPONSE;

int mil_read_response(const char* p_szPath, ST_RESPONSE* p_pstRsp, unsigned char* p_pUniqID);

__int64 mil_read_license(ST_RESPONSE*);

#ifdef __MI_SERVER__

int mil_create_response(
	const wchar_t* p_wszPath,
	int p_nProuct, int p_nOS,
	__int64 p_nExpire,
	unsigned char* p_pUniqID);

#endif//__MI_SERVER__
