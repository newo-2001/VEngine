#pragma comment (lib, "Setupapi.lib")

#include <iostream>
#include <vector>
#include <objbase.h>

#include "AudioDevices.h"
#include "DisplayDevices.h"

void Initialize()
{
    HRESULT result = CoInitialize(NULL);
}

void Uninitialize()
{
    CoUninitialize();
}

void ListConnectedDevices(DisplayDeviceManager& displayManager, AudioDeviceManager& audioManager)
{
    std::cout << "Detected display devices:" << std::endl;
    for (DisplayDevice& device : displayManager.DisplayDevices)
    {
        std::cout << "\t<" << device.GetDeviceId() << "> "
            << device.GetManufacturer() << ": "
            << device.GetDisplayName() << std::endl;
    }

    std::cout << std::endl << "Detected audio devices:" << std::endl;
    for (AudioDevice& device : audioManager.AudioDevices)
    {
        std::wcout << '\t' << device.GetDisplayName() << std::endl;
    }
}

void Run()
{
    AudioDeviceManager audioManager;
    DisplayDeviceManager displayManager;

    ListConnectedDevices(displayManager, audioManager);

    DisplayDevice device = displayManager.DisplayDevices[1];
    device.Detach();
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
