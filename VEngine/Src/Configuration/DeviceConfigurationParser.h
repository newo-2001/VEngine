#pragma once
#include <string>
#include <unordered_map>
#include "DeviceConfiguration.h"

template<typename T>
class IDeviceConfigurationParser
{
public:
    virtual DeviceConfigurationMap Parse(const T& data) const = 0;
};

class DeviceConfigurationParser : public IDeviceConfigurationParser<std::string>
{
public:
    DeviceConfigurationMap Parse(const std::string& data) const;
};
