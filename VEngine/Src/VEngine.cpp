#pragma comment (lib, "Setupapi.lib")

#include <iostream>
#include <vector>

#include "AudioDevices.h"
#include "DisplayDevices.h"
#include <objbase.h>

void initialize()
{
    HRESULT result = CoInitialize(NULL);
}

void uninitialize()
{
    CoUninitialize();
}

void run()
{
    std::cout << "Detected display devices:" << std::endl;
    std::vector<DisplayDevice> displayDevices = GetDisplayDevices();
    for (DisplayDevice& device : displayDevices)
    {
        std::cout << '\t' << device.Manufacturer << ": " << device.DisplayName << std::endl;
    }

    std::cout << std::endl << "Detected audio devices:" << std::endl;
    std::vector<AudioDevice> audioDevices = GetAudioDevices();
    for (AudioDevice& device : audioDevices)
    {
        std::wcout << '\t' << device.DisplayName << std::endl;
    }
}

int main()
{
    try
    {
        initialize();
        run();
        uninitialize();
    }
    catch (std::exception e)
    {
        std::cerr << "[Uncaught Exception] " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
