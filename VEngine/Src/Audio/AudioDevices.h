#pragma once
#include <string>
#include <vector>
#include <optional>
#include <mmdeviceapi.h>

class AudioDevice
{
public:
    AudioDevice(IMMDevice* device);
    ~AudioDevice();
    const std::wstring& GetDisplayName() const;
private:
    mutable std::optional<std::wstring> m_displayName;
    IMMDevice* m_device;
};

class AudioDeviceManager
{
public:
    AudioDeviceManager();
    ~AudioDeviceManager();

    std::vector<AudioDevice> AudioDevices;
private:
    IMMDeviceCollection* m_deviceCollection;
};
