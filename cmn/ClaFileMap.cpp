#include "pch.h"
#include "ClaFileMap.h"
#include "ClaSecurityDescriptor.h"

ClaFileMap::ClaFileMap()
 : _pMem(NULL)
 , _hFilemap(NULL)
{
}

ClaFileMap::~ClaFileMap()
{
	closeMap();
}

bool ClaFileMap::IsCreateMap(const wchar_t* p_wszName)
{
	HANDLE hMapTemp = OpenFileMapping(FILE_MAP_READ, FALSE, p_wszName);
	if (hMapTemp != INVALID_HANDLE_VALUE && hMapTemp != NULL) {
		CloseHandle(hMapTemp);
		return true;
	}
	return false;
}

unsigned int ClaFileMap::createMap(const wchar_t* p_wszName, int p_nSize, void** p_ppMem)
{
	closeMap();
	
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = ClaSecurityDescriptor::GetSecurityDescriptor(FILE_MAP_READ | FILE_MAP_WRITE);
	// Create file map
	_hFilemap = CreateFileMappingW(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, p_nSize, p_wszName);

	delete sa.lpSecurityDescriptor;

	if (_hFilemap == NULL) {
		return 1;
	}

	// Map file map to current process's address space
	_pMem = MapViewOfFile(_hFilemap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	if (_pMem == NULL) {
		return 2;
	}

	*p_ppMem = _pMem;
	return 0;
}

unsigned int ClaFileMap::openMap(const wchar_t* p_wszName, void** p_ppMem)
{
	closeMap();

	// Open existing file map
	_hFilemap = OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE, FALSE, p_wszName);

	if (_hFilemap == NULL) {
		return 1;
	}

	// Map file map to current process's address space
	_pMem = MapViewOfFile(_hFilemap, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0);

	if (_pMem == NULL) {
		return 2;
	}

	*p_ppMem = _pMem;
	return 0;
}

void ClaFileMap::closeMap()
{
	if (_pMem != NULL) {
		UnmapViewOfFile(_pMem);
		_pMem = NULL;
	}
	if (_hFilemap != NULL) {
		CloseHandle(_hFilemap);
		_hFilemap = NULL;
	}
}
