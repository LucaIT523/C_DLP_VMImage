#include "pch.h"
#include "ClaModelImage.h"

void ClaModelImage::test()
{
	m_strOS = L"Windows 7 x64";
	m_strName = L"Test VM Image";
	m_strPath = L"D:\\vm\\vm_win7\\Windows 7 x64.vmx";
	m_nStatus = image_status_runable;
	m_strPassword = L"1q1q!Q!Q";
}

CString ClaModelImage::toString()
{
	return L"";
}

int ClaModelImage::fromString()
{
	return 0;
}
