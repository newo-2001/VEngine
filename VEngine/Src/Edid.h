#pragma once
#include <string>

struct Edid
{
    std::string Manufacturer;
    std::string DisplayName;
};

void DecodeEdid(const char* data, size_t length, Edid& edid);
