#pragma once
#include <string>
#include <vector>

struct AudioDevice
{
    std::wstring DisplayName;
};

std::vector<AudioDevice> GetAudioDevices();
