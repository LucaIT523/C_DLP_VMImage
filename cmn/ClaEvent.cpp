#include "pch.h"
#include "ClaEvent.h"
#include "ClaSecurityDescriptor.h"

bool ClaEvent::IsCreate(const wchar_t* p_wszName)
{
	HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, p_wszName);
	if (hMutex != NULL && hMutex != INVALID_HANDLE_VALUE) {
		CloseHandle(hMutex);
		return true;
	}
	return false;
}

HANDLE ClaEvent::Create(const wchar_t* p_wszName)
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = ClaSecurityDescriptor::GetSecurityDescriptor(EVENT_ALL_ACCESS);
	// Create file map
	HANDLE hMutex = CreateEvent(&sa, FALSE, FALSE, p_wszName);

	delete sa.lpSecurityDescriptor;
	return hMutex;
}

HANDLE ClaEvent::Open(const wchar_t* p_wszName)
{
	HANDLE hMutex = OpenEvent(EVENT_ALL_ACCESS, FALSE, p_wszName);
	return hMutex;
}
