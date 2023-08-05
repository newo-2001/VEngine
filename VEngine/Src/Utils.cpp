#include "Utils.h"
#include <fstream>
#include <sstream>
#include <format>

std::string WStringToString(const std::wstring& wstr)
{
	std::string str;
	size_t size;
	str.resize(wstr.length());
	wcstombs_s(&size, &str[0], str.size() + 1, wstr.c_str(), wstr.size());
	return str;
}

std::wstring StringToWString(const std::string& str)
{
	std::wstring wstr;
	size_t size;
	wstr.resize(str.length());
	mbstowcs_s(&size, &wstr[0], wstr.size() + 1, str.c_str(), str.size());
	return wstr;
}

std::string ReadFileToString(const std::string& filePath)
{
	std::ifstream stream(filePath);
	if (!stream.is_open())
		throw std::exception(std::format("Failed to open file {}", filePath).c_str());

	std::stringstream strStream;
	strStream << stream.rdbuf();
	std::string str = strStream.str();

	return str;
}