#include <iostream>
#include <vector>
#include <format>

#include "Audio/AudioDevices.h"
#include "Displays/DisplayConfiguration.h"
#include "Configuration/DeviceConfiguration.h"
#include "Configuration/DeviceConfigurationParser.h"
#include "Utils.h"
#include "Arguments.h"

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

void ApplyConfiguration(const std::string& configurationFilePath, const std::string& configurationName)
{
    DeviceConfigurationMap configurations = ReadConfigurationsFromFile(configurationFilePath);
    if (!configurations.contains(configurationName))
    {
        throw std::exception(std::format("Undefined configuration: {}", configurationName).c_str());
    }

    DeviceConfiguration& config = configurations.at(configurationName);
    config.Apply();
}

void ShowHelp()
{
    std::cout << "Available arguments:" << std::endl
        << "--help: Show this menu" << std::endl
        << "-l: List all currently active devices" << std::endl
        << "-c: The name of the configuration to switch to" << std::endl
        << "-f: The path to the configuration file (default: devices.conf)" << std::endl;
}

void ListActiveDevices()
{
    DisplayConfiguration displays = DisplayConfiguration::Active();

    std::cout << "Active configuration:" << std::endl;
    displays.Print();
    std::cout << std::endl;

    std::cout << "Connected audio devices:" << std::endl;
    AudioDeviceManager audioDeviceManager;
    for (const auto& audioDevice : audioDeviceManager.AudioDevices)
    {
        std::wcout << '\t' << audioDevice.GetDisplayName() << " (" << audioDevice.GetId() << ')' << std::endl;
    }
}

void Run(const Arguments& arguments)
{
    switch (arguments.Action)
    {
    case Action::ListActiveDevices:
        return ListActiveDevices();
    case Action::ShowHelp:
        return ShowHelp();
    case Action::ApplyConfiguration:
        const std::string& configurationName = arguments.Configuration.value();
        return ApplyConfiguration(arguments.ConfigurationFilePath, configurationName);
    }
}

int main(int argc, char** argv)
{
    try
    {
        Arguments arguments(argc, argv);

        Initialize();
        Run(arguments);
        Uninitialize();
    }
    catch (std::exception e)
    {
        std::cerr << "[Uncaught Exception] " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
