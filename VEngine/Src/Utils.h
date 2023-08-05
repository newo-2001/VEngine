#pragma once
#include <string>

std::string WStringToString(const std::wstring& wstr);
std::wstring StringToWString(const std::string& str);

std::string ReadFileToString(const std::string& filePath);
