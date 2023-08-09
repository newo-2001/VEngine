#include "DeviceConfiguration.h"
#include <iostream>

DeviceConfiguration::DeviceConfiguration(std::string name) { }
DeviceConfiguration::DeviceConfiguration()
    : DeviceConfiguration("Default") { }

void DeviceConfiguration::Print() const
{
    if (Displays.has_value())
    {
        Displays.value().Print();
        std::cout << std::endl;
    }
    if (Audio.has_value())
    {
        Audio.value().Print();
        std::cout << std::endl;
    }
}

void DeviceConfiguration::Apply()
{
    if (Displays.has_value()) Displays.value().Apply();
    if (Audio.has_value())
    {
        // If displays were switch we should wait for them to be connected
        // as they might connect the requested primary audio output device
        if (Displays.has_value())
        {
            // Should be a better way of detecting a completed switch
            Sleep(8000);
        }

        Audio.value().Apply();
    }
}