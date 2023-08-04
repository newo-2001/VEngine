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
    }

    enumerator->Release();
}

AudioDeviceManager::~AudioDeviceManager()
{
    m_deviceCollection->Release();
}

AudioDevice::AudioDevice(IMMDevice* device)
    : m_device(device) { }

AudioDevice::~AudioDevice()
{
    m_device->Release();
}

const std::wstring& AudioDevice::GetDisplayName() const
{
    if (m_displayName) return m_displayName.value();

    IPropertyStore* properties = nullptr;
    HRESULT status = m_device->OpenPropertyStore(STGM_READ, &properties);

    PROPVARIANT variant;
    status = properties->GetValue(PKEY_Device_FriendlyName, &variant);
    
    m_displayName = variant.vt == VT_EMPTY
        ? L"Nameless Audio Device"
        : variant.pwszVal;

    properties->Release();

    return m_displayName.value();
}