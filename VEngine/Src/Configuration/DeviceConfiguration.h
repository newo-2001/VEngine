#pragma once
#include <unordered_map>
#include <string>
#include "../Displays/DisplayConfiguration.h"

class DeviceConfiguration
{
public:
    DeviceConfiguration(std::string name, DisplayConfiguration displayConfig);

    void Print() const;

    const std::string& GetName() const { return m_name; }

    DisplayConfiguration Displays;
private:
    std::string m_name;
};

using DeviceConfigurationMap = std::unordered_map<std::string, DeviceConfiguration>;
