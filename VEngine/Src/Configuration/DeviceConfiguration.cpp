#include "DeviceConfiguration.h"
#include <iostream>

DeviceConfiguration::DeviceConfiguration(std::string name, DisplayConfiguration displayConfig)
    : Displays(displayConfig), m_name(name) { }

void DeviceConfiguration::Print() const
{
    Displays.Print();
}