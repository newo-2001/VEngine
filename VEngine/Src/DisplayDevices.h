#pragma once

#include <string>
#include <vector>
#include <optional>
#include <Windows.h>
#include <SetupAPI.h>

class DisplayDevice
{
public:
    DisplayDevice(HDEVINFO deviceInfo, SP_DEVINFO_DATA deviceInfoData);

    std::string GetManufacturer() const;
    const std::string& GetDisplayName() const;
private:
    mutable std::optional<std::string> m_displayName;
    char m_edid[1024];
};

class DisplayDeviceManager
{
public:
    DisplayDeviceManager();
    std::vector<DisplayDevice> DisplayDevices;
};

