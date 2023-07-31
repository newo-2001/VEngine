#pragma once
#include <string>
#include <vector>

struct DisplayDevice
{
    std::string Manufacturer;
    std::string DisplayName;
};

std::vector<DisplayDevice> GetDisplayDevices();
