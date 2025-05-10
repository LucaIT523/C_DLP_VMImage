#include "pch.h"
#include "ClaFileProperty.h"

#pragma comment(lib, "Version.lib")

ClaFileProperty::ClaFileProperty()
{

}

ClaFileProperty::~ClaFileProperty()
{

}

CString ClaFileProperty::GetProperty(const wchar_t* p_wszPath, const wchar_t* p_wszPropertyName)
{
    DWORD dummy;
    DWORD size = GetFileVersionInfoSize(p_wszPath, &dummy);
    if (size == 0) {
        return L"";
    }

    unsigned char* pBuff = (unsigned char*)malloc(size);
    if (!GetFileVersionInfo(p_wszPath, 0, size, pBuff)) {
        free(pBuff);
        return L"";
    }

    UINT cbTranslate;
    struct LANGANDCODEPAGE {
        WORD wLanguage;
        WORD wCodePage;
    } *lpTranslate;

    // Read the list of languages and code pages.
    VerQueryValue(pBuff,
        TEXT("\\VarFileInfo\\Translation"),
        (LPVOID*)&lpTranslate,
        &cbTranslate);

    void* buffer;
    UINT len;
    wchar_t wszPropertyName[MAX_PATH]; memset(wszPropertyName, 0, sizeof(wszPropertyName));
    swprintf_s(wszPropertyName, MAX_PATH, L"\\StringFileInfo\\%04x%04x\\%s", 
        lpTranslate->wLanguage, lpTranslate->wCodePage, p_wszPropertyName);
    if (!VerQueryValue(pBuff, wszPropertyName, &buffer, &len)) {
        swprintf_s(wszPropertyName, MAX_PATH, L"\\StringFileInfo\\040904E4\\%s", p_wszPropertyName);
        if (!VerQueryValue(pBuff, wszPropertyName, &buffer, &len)) {

            free(pBuff);
            return L"";
        }
    }

    CString ret = (wchar_t*)buffer;

    free(pBuff);
    return ret;
}