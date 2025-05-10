#include "pch.h"
#include "ClaMutex.h"
#include "ClaSecurityDescriptor.h"

bool ClaMutex::IsCreate(const wchar_t* p_wszName)
{
	HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, p_wszName);
	if (hMutex != NULL && hMutex != INVALID_HANDLE_VALUE) {
		CloseHandle(hMutex);
		return true;
	}
	return false;
}

HANDLE ClaMutex::Create(const wchar_t* p_wszName)
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = ClaSecurityDescriptor::GetSecurityDescriptor(MUTEX_ALL_ACCESS);
	// Create file map
	HANDLE hMutex = CreateMutex(&sa, TRUE, p_wszName);

	delete sa.lpSecurityDescriptor;
	return hMutex;
}
