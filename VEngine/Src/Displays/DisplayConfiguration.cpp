#include "DisplayConfiguration.h"
#include <format>
#include <iostream>
#include <sstream>
#include <Windows.h>
#include "../Utils.h"

DisplayConfiguration::DisplayConfiguration() { }

DisplayConfiguration::DisplayConfiguration(std::vector<DisplayDevice> enabledDisplays)
    : EnabledDisplays(enabledDisplays) { }

DisplayConfiguration DisplayConfiguration::Active()
{
    std::vector<DisplayDevice> enabledDevices;

    UINT32 pathAmount, modeAmount;
    LONG status = GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathAmount, &modeAmount);
    enabledDevices.reserve(pathAmount);

    auto paths = new DISPLAYCONFIG_PATH_INFO[pathAmount];
    auto modes = new DISPLAYCONFIG_MODE_INFO[modeAmount];

    status = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathAmount, paths, &modeAmount, modes, nullptr);

    for (size_t i = 0; i < pathAmount; i++)
    {
        DISPLAYCONFIG_PATH_INFO& path = paths[i];

        DISPLAYCONFIG_TARGET_DEVICE_NAME targetDeviceName {};
        targetDeviceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        targetDeviceName.header.adapterId = path.targetInfo.adapterId;
        targetDeviceName.header.id = path.targetInfo.id;
        targetDeviceName.header.size = sizeof(targetDeviceName);
        status = DisplayConfigGetDeviceInfo(&targetDeviceName.header);

        DISPLAYCONFIG_SOURCE_DEVICE_NAME sourceDeviceName {};
        sourceDeviceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        sourceDeviceName.header.adapterId = path.sourceInfo.adapterId;
        sourceDeviceName.header.id = path.sourceInfo.id;
        sourceDeviceName.header.size = sizeof(sourceDeviceName);
        status = DisplayConfigGetDeviceInfo(&sourceDeviceName.header);

        DisplayDevice display(
            path.targetInfo.id,
            targetDeviceName.monitorFriendlyDeviceName,
            path.sourceInfo.id
        );

        enabledDevices.push_back(display);
    }

    delete[] paths;
    delete[] modes;

    return DisplayConfiguration(enabledDevices);
}

void DisplayConfiguration::Apply()
{
    if (EnabledDisplays.empty()) throw std::exception("Can't disable all displays");

    UINT32 pathAmount, modeAmount;
    LONG status = GetDisplayConfigBufferSizes(QDC_ALL_PATHS, &pathAmount, &modeAmount);

    auto paths = new DISPLAYCONFIG_PATH_INFO[pathAmount];
    auto modes = new DISPLAYCONFIG_MODE_INFO[modeAmount];

    status = QueryDisplayConfig(QDC_ALL_PATHS, &pathAmount, paths, &modeAmount, modes, nullptr);

    bool* updated = new bool[pathAmount] { false };

    for (const DisplayDevice& device : EnabledDisplays)
    {
        if (!device.GetSourceId().has_value())
            throw new std::exception("Attempted to connect display without source");

        bool found = false;
        for (size_t i = 0; i < pathAmount; i++)
        {
            DISPLAYCONFIG_PATH_INFO& path = paths[i];

            if (device.GetTargetId() == path.targetInfo.id
                && device.GetSourceId().value() == path.sourceInfo.id)
            {
                std::wcout << std::format(L"[INFO] Enabling display {}", device.GetDisplayName()) << std::endl;
                path.flags |= DISPLAYCONFIG_PATH_ACTIVE;
                updated[i] = true;
                found = true;
                break;
            }
        }

        if (!found) throw std::exception("Supplied display path was not found");
    }

    DisplayConfiguration active = DisplayConfiguration::Active();

    for (size_t i = 0; i < pathAmount; i++)
    {
        if (updated[i]) continue;

        auto& path = paths[i];
        if (path.flags & DISPLAYCONFIG_PATH_ACTIVE)
        {
            std::wstring displayName = active.FindDisplayByTargetId(path.targetInfo.id).GetDisplayName();
            std::wcout << std::format(L"[INFO] Disabling display {}", displayName) << std::endl;
            path.flags &= ~DISPLAYCONFIG_PATH_ACTIVE;
        }
    }

    delete[] updated;

    constexpr UINT32 FLAGS = SDC_APPLY | SDC_USE_SUPPLIED_DISPLAY_CONFIG;
    status = SetDisplayConfig(pathAmount, paths, modeAmount, modes, FLAGS);

    delete[] paths;
    delete[] modes;
}

const DisplayDevice& DisplayConfiguration::FindDisplayByName(std::wstring displayName) const
{
    auto display = std::find_if(EnabledDisplays.begin(), EnabledDisplays.end(),
                 [displayName] (const DisplayDevice& device) { return device.GetDisplayName() == displayName; });

    if (display == EnabledDisplays.end())
    {
        std::string displayNameA = WStringToString(displayName);
        throw std::exception(std::format("Display with name {} does not exist", displayNameA).c_str());
    }

    return *display;
}

const DisplayDevice& DisplayConfiguration::FindDisplayByTargetId(unsigned int targetId) const
{
    auto display = std::find_if(EnabledDisplays.begin(), EnabledDisplays.end(),
                 [targetId] (const DisplayDevice& device) { return device.GetTargetId() == targetId; });

    if (display == EnabledDisplays.end())
        throw std::exception(std::format("Display with targetId {} does not exist", std::to_string(targetId)).c_str());

    return *display;
}

void DisplayConfiguration::Print() const
{
    std::cout << "Display devices:" << std::endl;
    for (const DisplayDevice& device : EnabledDisplays)
    {
        std::wcout << '\t' << device.GetDisplayName() << " (" << std::to_wstring(device.GetTargetId()) << ")";
        if (device.GetSourceId().has_value())
        {
            std::cout << " @ " << std::to_string(device.GetSourceId().value());
        }

        std::cout << std::endl;
    }
}