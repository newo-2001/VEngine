#pragma once

#include <string>
#include <vector>
#include <array>
#include <optional>
#include <Windows.h>
#include <SetupAPI.h>

class DisplayDevice
{
public:
    DisplayDevice(std::string deviceId, std::array<char, 1024> edid);
    
    std::string GetManufacturer() const;
    const std::string& GetDeviceId() const { return m_deviceId; };
    const std::string& GetDisplayName() const;
private:
    mutable std::optional<std::string> m_displayName;
    std::array<char, 1024> m_edid;
    std::string m_deviceId;
};

class DisplayDeviceManager
{
public:
    DisplayDeviceManager();
    std::vector<DisplayDevice> DisplayDevices;
};

