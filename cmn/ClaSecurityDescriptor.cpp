#include "pch.h"
#include "ClaSecurityDescriptor.h"
#include <AclAPI.h>

ClaSecurityDescriptor::ClaSecurityDescriptor()
{
}

ClaSecurityDescriptor::~ClaSecurityDescriptor()
{
}

PSECURITY_DESCRIPTOR ClaSecurityDescriptor::GetSecurityDescriptor(DWORD p_dwPermission)
{
	SECURITY_DESCRIPTOR *sd = new SECURITY_DESCRIPTOR;
	InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION);

	PACL pDACL = NULL;
	EXPLICIT_ACCESS ea;
	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
	ea.grfAccessPermissions = p_dwPermission;
	ea.grfAccessMode = SET_ACCESS;
	ea.grfInheritance = NO_INHERITANCE;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
	ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea.Trustee.ptstrName = TEXT("EVERYONE");

	SetEntriesInAcl(1, &ea, NULL, &pDACL);

	SetSecurityDescriptorDacl(sd, TRUE, pDACL, FALSE);
	return sd;

//     PSECURITY_DESCRIPTOR pSecurityDescriptor = NULL;
//     if (!ConvertStringSecurityDescriptorToSecurityDescriptor(
//         L"D:(A;;0x1;;;WD)(A;;0x1;;;RU)",
//         SDDL_REVISION_1,
//         &pSecurityDescriptor,
//         NULL)) {
//         //std::cerr << "Error: ConvertStringSecurityDescriptorToSecurityDescriptor failed: " << GetLastError() << std::endl;
//         return NULL;
//     }
// 
//     // Grant FILE_MAP_ALL_ACCESS right (read/write) to the security descriptor
//     PACL pAcl = NULL;
//     EXPLICIT_ACCESS ea;
//     ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
//     ea.grfAccessPermissions = p_dwPermission;
//     ea.grfAccessMode = SET_ACCESS;
//     ea.grfInheritance = NO_INHERITANCE;
//     ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
//     ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
//     ea.Trustee.ptstrName = (LPTSTR)(TEXT("Everyone"));
// 
//     if (SetEntriesInAcl(1, &ea, NULL, &pAcl) != ERROR_SUCCESS) {
//         LocalFree(pSecurityDescriptor);
// //        std::cerr << "Error: SetEntriesInAcl failed: " << GetLastError() << std::endl;
//         return NULL;
//     }
// 
//     // Set the ACL to the security descriptor
//     if (!SetSecurityDescriptorDacl(pSecurityDescriptor, TRUE, pAcl, FALSE)) {
//         LocalFree(pAcl);
//         LocalFree(pSecurityDescriptor);
// //        std::cerr << "Error: SetSecurityDescriptorDacl failed: " << GetLastError() << std::endl;
//         return NULL;
//     }
// 
//     // Use the modified security descriptor with FILE_MAP_ALL_ACCESS here
// 
//     // Clean up
//     LocalFree(pAcl);
//     return pSecurityDescriptor;
//    LocalFree(pSecurityDescriptor);

}
