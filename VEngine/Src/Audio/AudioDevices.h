#pragma once
#include <string>
#include <vector>
#include <optional>
#include <mmdeviceapi.h>

class AudioDevice
{
public:
    AudioDevice(IMMDevice* device);
    AudioDevice(std::wstring name, std::wstring id);

    const std::wstring& GetDisplayName() const { return m_displayName; }
    const std::wstring& GetId() const { return m_id; }
private:
    std::wstring m_displayName;
    std::wstring m_id;
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
