#pragma once
#include <unordered_map>
#include <string>
#include <optional>
#include "../Displays/DisplayConfiguration.h"
#include "../Audio/AudioConfiguration.h"

class DeviceConfiguration
{
public:
    DeviceConfiguration();
    DeviceConfiguration(std::string name);

    void Print() const;
    void Apply();

    std::optional<DisplayConfiguration> Displays;
    std::optional<AudioConfiguration> Audio;
    std::string Name;
};

using DeviceConfigurationMap = std::unordered_map<std::string, DeviceConfiguration>;
