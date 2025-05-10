#pragma once

void kn_encrypt(unsigned char* p_pBuff, DWORD p_cchBuff);
void kn_decrypt(unsigned char* p_pBuff, DWORD p_cchBuff);

void kn_encrypt_with_pwd(unsigned char* p_pBuff, DWORD p_cchBuff, const wchar_t* p_pKey);
void kn_decrypt_with_pwd(unsigned char* p_pBuff, DWORD p_cchBuff, const wchar_t* p_pKey);

#define GD_MARK_CRYPT_FILE	'6587'
typedef struct tagFileCrypto {
	unsigned long m_lMark;	// '6587'
	int m_lSize;
	unsigned char m_pPwdHash[32];
	unsigned char m_pUserData[256];
	unsigned char m_pData[1];
}ST_FILE_CRYPTO;

BOOL kn_is_encrypt_file(const wchar_t* p_pPathSrc);
int kn_encrypt_file_with_pwd(
	const wchar_t* p_pPathSrc, const wchar_t* p_pPathDst, 
	const wchar_t* p_pKey = L"KENAN-ENCRYT-FILE", 
	const wchar_t* p_wszUserData = L"");
int kn_reencrypt_file_with_pwd(
	const wchar_t* p_pPathSrc, 
	const wchar_t* p_pPathDst, 
	const wchar_t* p_wszOld, 
	const wchar_t* p_wszNew, 
	const wchar_t* p_wszUserData);
int kn_decrypt_file_with_pwd(
	const wchar_t* p_pPathSrc, const wchar_t* p_pPathDst, 
	const wchar_t* p_pKey = L"KENAN-ENCRYT-FILE", 
	wchar_t* p_wszUserData = NULL, int p_cchUserData = 0);
