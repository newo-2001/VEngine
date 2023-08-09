#include "AudioDevices.h"

#include <Windows.h>
#include <Functiondiscoverykeys_devpkey.h>

AudioDeviceManager::AudioDeviceManager()
    : m_deviceCollection(nullptr)
{
    IMMDeviceEnumerator* enumerator = nullptr;
    HRESULT status = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), (void**) &enumerator);

    status = enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &m_deviceCollection);

    UINT amount;
    status = m_deviceCollection->GetCount(&amount);
    AudioDevices.reserve(amount);

    for (UINT index = 0; index < amount; index++)
    {
        IMMDevice* device = nullptr;
        HRESULT status = m_deviceCollection->Item(index, &device);
        
        AudioDevices.push_back(AudioDevice(device));
        device->Release();
    }

    enumerator->Release();
}

AudioDeviceManager::~AudioDeviceManager()
{
    m_deviceCollection->Release();
}

std::wstring GetIdForDevice(IMMDevice* device)
{
    wchar_t* id = nullptr;
    device->GetId(&id);
    return std::wstring(id);
}

std::wstring GetDisplayNameForDevice(IMMDevice* device)
{
    IPropertyStore* properties = nullptr;
    HRESULT status = device->OpenPropertyStore(STGM_READ, &properties);

    PROPVARIANT variant;
    status = properties->GetValue(PKEY_Device_FriendlyName, &variant);
    
    std::wstring displayName = variant.vt == VT_EMPTY
        ? L"Nameless Audio Device"
        : variant.pwszVal;

    properties->Release();

    return displayName;
}

AudioDevice::AudioDevice(IMMDevice* device)
{
    m_displayName = GetDisplayNameForDevice(device);
    m_id = GetIdForDevice(device);
}

AudioDevice::AudioDevice(std::wstring name, std::wstring id)
    : m_id(id), m_displayName(name) { }
