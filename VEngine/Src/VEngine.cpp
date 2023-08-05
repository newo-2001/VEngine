#include <iostream>
#include <vector>
#include <objbase.h>

#include "Audio/AudioDevices.h"
#include "Displays/DisplayConfiguration.h"
#include "Configuration/DeviceConfiguration.h"
#include "Configuration/DeviceConfigurationParser.h"
#include "Utils.h"

void Initialize()
{
    HRESULT result = CoInitialize(NULL);
}

void Uninitialize()
{
    CoUninitialize();
}

DeviceConfigurationMap ReadConfigurationsFromFile(const std::string& filePath)
{
    std::string content = ReadFileToString(filePath);
    std::unique_ptr<IDeviceConfigurationParser<std::string>> parser = std::make_unique<DeviceConfigurationParser>();
    return parser->Parse(content);
}

void Run()
{
    DisplayConfiguration activeConfig = DisplayConfiguration::Active();

    std::cout << "Active display configuration:" << std::endl;
    activeConfig.Print();
    std::cout << std::endl;

    DeviceConfigurationMap configurations = ReadConfigurationsFromFile("deviceconfigurations.conf");
    for (const auto& [name, configuration] : configurations)
    {
        std::cout << '[' << name << ']' << std::endl;
        configuration.Print();
        std::cout << std::endl;
    }
}

int main()
{
    try
    {
        Initialize();
        Run();
        Uninitialize();
    }
    catch (std::exception e)
    {
        std::cerr << "[Uncaught Exception] " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
