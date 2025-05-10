#include "pch.h"
#include "kncrypto.h"
#include "AES.h"
#include "SHA256.h"

void kn_encrypt(unsigned char* p_pBuff, DWORD p_cchBuff)
{
	unsigned char pKey[32]; memset(pKey, 0, sizeof(pKey));

	for (int i = 0; i < p_cchBuff; i++) {
		p_pBuff[i] = p_pBuff[i] ^ pKey[i % 32];
	}
}

void kn_decrypt(unsigned char* p_pBuff, DWORD p_cchBuff)
{
	unsigned char pKey[32]; memset(pKey, 0, sizeof(pKey));

	for (int i = 0; i < p_cchBuff; i++) {
		p_pBuff[i] = p_pBuff[i] ^ pKey[i % 32];
	}
}

void kn_encrypt_with_pwd(unsigned char* p_pBuff, DWORD p_cchBuff, const wchar_t* p_pKey)
{
	SHA256 sha;
	wchar_t wszPwd[MAX_PATH]; memset(wszPwd, 0, sizeof(wszPwd));
	wcscpy_s(wszPwd, wcslen(p_pKey)+1, p_pKey);
	sha.update((const uint8_t*)wszPwd, sizeof(wszPwd));
	unsigned char pKey[32];
	memcpy(pKey, sha.digest().data(), 32);
	unsigned char pIV[32];
	sha.update("iv-pwd");
	memcpy(pIV, sha.digest().data(), 32);

	AES aes;

	unsigned char* p = aes.EncryptCBC(p_pBuff, (int(p_cchBuff / 32)) * 32, pKey, pIV);
	memcpy(p_pBuff, p, (int(p_cchBuff / 32)) * 32);
	int nLeft = p_cchBuff % 32;
	for (int i = 0; i < nLeft; i++) {
		p_pBuff[i + (int(p_cchBuff / 32)) * 32] ^= pKey[i];
	}
	delete[] p;
}

void kn_decrypt_with_pwd(unsigned char* p_pBuff, DWORD p_cchBuff, const wchar_t* p_pKey)
{
	SHA256 sha;
	wchar_t wszPwd[MAX_PATH]; memset(wszPwd, 0, sizeof(wszPwd));
	wcscpy_s(wszPwd, wcslen(p_pKey) + 1, p_pKey);
	sha.update((const uint8_t*)wszPwd, sizeof(wszPwd));
	unsigned char pKey[32];
	memcpy(pKey, sha.digest().data(), 32);
	unsigned char pIV[32];
	sha.update("iv-pwd");
	memcpy(pIV, sha.digest().data(), 32);

	AES aes;
	unsigned char* p = aes.DecryptCBC(p_pBuff, (int(p_cchBuff / 32)) * 32, pKey, pIV);
	memcpy(p_pBuff, p, (int(p_cchBuff / 32)) * 32);

	int nLeft = p_cchBuff % 32;
	for (int i = 0; i < nLeft; i++) {
		p_pBuff[i+ (int(p_cchBuff / 32)) * 32] ^= pKey[i];
	}
	delete[] p;
}

long getFileSize(const wchar_t* fileName) {
	struct _stat fileInfo;

	// Get file information
	if (_wstat(fileName, &fileInfo) != 0) {
		std::wcerr << L"Error getting file information: " << fileName << std::endl;
		return -1; // Return -1 to indicate an error
	}

	// Return the size of the file
	return fileInfo.st_size;
}

void getHash(unsigned char* p_pBuff, int p_cchBuff, unsigned char* p_pHash)
{
	SHA256 sha;
	sha.update(p_pBuff, p_cchBuff);
	memcpy(p_pHash, sha.digest().data(), 32);
}

BOOL kn_is_encrypt_file(const wchar_t* p_pPathSrc)
{
	long lSize = getFileSize(p_pPathSrc);
	if (lSize < sizeof(ST_FILE_CRYPTO)) return false;
	ST_FILE_CRYPTO stFile;

	FILE* pFile;
	_wfopen_s(&pFile, p_pPathSrc, L"rb");
	if (pFile == NULL) {
		return -2;
	}

	fread(&stFile, 1, sizeof(ST_FILE_CRYPTO), pFile);
	fclose(pFile);
	return stFile.m_lMark == GD_MARK_CRYPT_FILE;
}

int kn_encrypt_file_with_pwd(const wchar_t* p_pPathSrc, const wchar_t* p_pPathDst, const wchar_t* p_pKey, const wchar_t* p_wszUserData)
{
	ST_FILE_CRYPTO* pFileCrypto;
	long lSize = getFileSize(p_pPathSrc);
	if (lSize < 2) return -1;
	if (kn_is_encrypt_file(p_pPathSrc) == TRUE) return 0;

	pFileCrypto = (ST_FILE_CRYPTO*)malloc(sizeof(ST_FILE_CRYPTO) + lSize);
	memset(pFileCrypto, 0, sizeof(ST_FILE_CRYPTO) + lSize);

	pFileCrypto->m_lMark = GD_MARK_CRYPT_FILE;
	pFileCrypto->m_lSize = lSize;
	getHash((unsigned char*)p_pKey, wcslen(p_pKey) * 2, pFileCrypto->m_pPwdHash);
	FILE* pFile;
	_wfopen_s(&pFile, p_pPathSrc, L"rb");
	if (pFile == NULL) {
		free(pFileCrypto);
		return -2;
	}

	fread(pFileCrypto->m_pData, 1, lSize, pFile);
	fclose(pFile);
	kn_encrypt_with_pwd(pFileCrypto->m_pData, lSize, p_pKey);
	if (p_wszUserData != NULL) {
		wcscpy_s((wchar_t*)pFileCrypto->m_pUserData, 128, p_wszUserData);
	}
	kn_encrypt_with_pwd(pFileCrypto->m_pUserData, 256, p_pKey);

	_wfopen_s(&pFile, p_pPathDst, L"wb");
	if (pFile == NULL) {
		free(pFileCrypto);
		return -3;
	}
	fwrite(pFileCrypto, 1, sizeof(ST_FILE_CRYPTO) + lSize, pFile);
	free(pFileCrypto);
	fclose(pFile);
	return 0;
}

int kn_reencrypt_file_with_pwd(const wchar_t* p_pPathSrc, const wchar_t* p_pPathDst, const wchar_t* p_wszOld, const wchar_t* p_wszNew, const wchar_t* p_wszUserData)
{
	unsigned char pHash[32];
	ST_FILE_CRYPTO* pFileCrypto;
	long lSize = getFileSize(p_pPathSrc);
	if (lSize < sizeof(ST_FILE_CRYPTO)) return -1;

	pFileCrypto = (ST_FILE_CRYPTO*)malloc(lSize);
	memset(pFileCrypto, 0, lSize);

	FILE* pFile;
	_wfopen_s(&pFile, p_pPathSrc, L"rb");
	if (pFile == NULL) {
		free(pFileCrypto);
		return -2;
	}

	fread(pFileCrypto, 1, lSize, pFile);
	fclose(pFile);

	if (pFileCrypto->m_lMark != GD_MARK_CRYPT_FILE) {
		free(pFileCrypto);
		return -3;
	}
	if (pFileCrypto->m_lSize + sizeof(ST_FILE_CRYPTO) != lSize) {
		free(pFileCrypto);
		return -4;
	}

	getHash((unsigned char*)p_wszOld, wcslen(p_wszOld) * 2, pHash);
	if (memcmp(pHash, pFileCrypto->m_pPwdHash, 32) != 0) {
		free(pFileCrypto);
		return -5;
	}

	kn_decrypt_with_pwd(pFileCrypto->m_pData, pFileCrypto->m_lSize, p_wszOld);
	kn_decrypt_with_pwd(pFileCrypto->m_pUserData, 256, p_wszOld);

	kn_encrypt_with_pwd(pFileCrypto->m_pData, pFileCrypto->m_lSize, p_wszNew);
	if (p_wszUserData != NULL)
		wcscpy_s((wchar_t*)pFileCrypto->m_pUserData, 128, p_wszUserData);
	kn_encrypt_with_pwd(pFileCrypto->m_pUserData, 256, p_wszNew);

	getHash((unsigned char*)p_wszNew, wcslen(p_wszNew) * 2, pFileCrypto->m_pPwdHash);

	DeleteFile(p_pPathDst);
	_wfopen_s(&pFile, p_pPathDst, L"wb");
	if (pFile == NULL) {
		free(pFileCrypto);
		return -3;
	}
	fwrite(pFileCrypto, 1, lSize, pFile);
	free(pFileCrypto);
	fclose(pFile);
	return 0;
}

int kn_decrypt_file_with_pwd(const wchar_t* p_pPathSrc, const wchar_t* p_pPathDst, const wchar_t* p_pKey, wchar_t* p_wszUserData, int p_cchUserData)
{
	unsigned char pHash[32];
	ST_FILE_CRYPTO* pFileCrypto;
	long lSize = getFileSize(p_pPathSrc);
	if (lSize < sizeof(ST_FILE_CRYPTO)) return -1;

	pFileCrypto = (ST_FILE_CRYPTO*)malloc(lSize);
	memset(pFileCrypto, 0, lSize);
	
	FILE* pFile;
	_wfopen_s(&pFile, p_pPathSrc, L"rb");
	if (pFile == NULL) {
		free(pFileCrypto);
		return -2;
	}

	fread(pFileCrypto, 1, lSize, pFile);
	fclose(pFile);

	if (pFileCrypto->m_lMark != GD_MARK_CRYPT_FILE) {
		free(pFileCrypto);
		return -3;
	}
	if (pFileCrypto->m_lSize + sizeof(ST_FILE_CRYPTO) != lSize) {
		free(pFileCrypto);
		return -4;
	}

	getHash((unsigned char*)p_pKey, wcslen(p_pKey) * 2, pHash);
	if (memcmp(pHash, pFileCrypto->m_pPwdHash, 32) != 0) {
		free(pFileCrypto);
		return -5;
	}

	kn_decrypt_with_pwd(pFileCrypto->m_pData, pFileCrypto->m_lSize, p_pKey);
	kn_decrypt_with_pwd(pFileCrypto->m_pUserData, 256, p_pKey);
	if (p_wszUserData)
		wcsncpy_s(p_wszUserData, p_cchUserData, (wchar_t*)pFileCrypto->m_pUserData, p_cchUserData - 1);

	DeleteFile(p_pPathDst);
	_wfopen_s(&pFile, p_pPathDst, L"wb");
	if (pFile == NULL) {
		free(pFileCrypto);
		return -3;
	}
	fwrite(pFileCrypto->m_pData, 1, pFileCrypto->m_lSize, pFile);
	free(pFileCrypto);
	fclose(pFile);
	return 0;

}
