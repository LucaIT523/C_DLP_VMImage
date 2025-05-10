#include "pch.h"
#include "ClaVM.h"
#include "ClaRegMgr.h"

bool IsInsideVMWare()
{
#ifndef WIN64
	bool rc = true;

	__try
	{
		__asm
		{
			push   edx
			push   ecx
			push   ebx

			mov    eax, 'VMXh'
			mov    ebx, 0 // any value but not the MAGIC VALUE

			mov    ecx, 10 // get VMWare version

			mov    edx, 'VX' // port number


			in     eax, dx // read port

			// on return EAX returns the VERSION

			cmp    ebx, 'VMXh' // is it a reply from VMWare?

			setz[rc] // set return value


			pop    ebx
			pop    ecx
			pop    edx
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		rc = false;
	}

	return rc;
#else
	wchar_t wszManufacture[MAX_PATH];

	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	reg.readStringW(L"HARDWARE\\DESCRIPTION\\System\\BIOS", L"SystemManufacturer", wszManufacture, MAX_PATH);
	if (wcsstr(wszManufacture, L"VMware") != NULL) {
		return true;
	}
	return false;
#endif
}

BOOL ClaVM::is_on_vm() {
	return IsInsideVMWare();
}