#pragma comment (lib, "Setupapi.lib")

#include <iostream>
#include <vector>
#include <objbase.h>

#include "Audio/AudioDevices.h"
#include "Displays/DisplayConfiguration.h"

void Initialize()
{
    HRESULT result = CoInitialize(NULL);
}

void Uninitialize()
{
    CoUninitialize();
}

void ListConnectedDevices(DisplayConfiguration& displayConfig, AudioDeviceManager& audioManager)
{
    std::cout << "Detected display devices:" << std::endl;
    for (DisplayDevice& device : displayConfig.EnabledDisplays)
    {
        std::wcout << '\t' << device.GetDisplayName() << " (" << std::to_wstring(device.GetTargetId()) << ")";
        if (device.GetSourceId().has_value())
        {
            std::cout << " @ " << std::to_string(device.GetSourceId().value());
        }

        std::cout << std::endl;
    }

    std::cout << std::endl << "Detected audio devices:" << std::endl;
    for (AudioDevice& device : audioManager.AudioDevices)
    {
        std::wcout << '\t' << device.GetDisplayName() << std::endl;
    }

    std::cout << std::endl;
}

void Run()
{
    AudioDeviceManager audioManager;
    DisplayConfiguration displayConfig = DisplayConfiguration::Active();

    ListConnectedDevices(displayConfig, audioManager);

    std::vector<DisplayDevice> enabled;
    
    enabled.push_back(displayConfig.FindDisplayByName(L"G2460"));
    
    DisplayDevice device(41220, L"2470W", 1);
    enabled.push_back(device);
    
    enabled.push_back(displayConfig.FindDisplayByName(L"Beyond TV"));

    displayConfig.Apply();
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
