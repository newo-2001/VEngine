#pragma comment (lib, "Setupapi.lib")

#include <iostream>
#include <vector>

#include "AudioDevices.h"
#include "DisplayDevices.h"
#include <objbase.h>

void Initialize()
{
    HRESULT result = CoInitialize(NULL);
}

void Uninitialize()
{
    CoUninitialize();
}

void Run()
{
    std::cout << "Detected display devices:" << std::endl;
    DisplayDeviceManager displayManager;
    for (DisplayDevice& device : displayManager.DisplayDevices)
    {
        std::cout << '\t' << device.GetManufacturer() << ": " << device.GetDisplayName() << std::endl;
    }

    std::cout << std::endl << "Detected audio devices:" << std::endl;
    AudioDeviceManager audioManager;
    for (AudioDevice& device : audioManager.AudioDevices)
    {
        std::wcout << '\t' << device.GetDisplayName() << std::endl;
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
