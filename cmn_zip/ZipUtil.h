#pragma once


#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <codecvt>


void	MyZipData(std::wstring	p_DataPath, std::wstring	p_ZipFilePath);

int		MyUnzipData(const std::string& zipPath, const std::string& extractPath);

std::string ConvertWStringToString(const std::wstring& wstr);