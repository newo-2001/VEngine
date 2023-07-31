#include "AudioDevices.h"

#include <Windows.h>
#include <mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>

void  GetAudioDevice(IMMDeviceCollection* collection, UINT index, AudioDevice& audioDevice)
{
    IMMDevice* device = nullptr;
    HRESULT status = collection->Item(index, &device);
    
    IPropertyStore* properties = nullptr;
    status = device->OpenPropertyStore(STGM_READ, &properties);

    PROPVARIANT variant;
    status = properties->GetValue(PKEY_Device_FriendlyName, &variant);

    audioDevice.DisplayName = variant.vt == VT_EMPTY
        ? L"Nameless Audio Device"
        : variant.pwszVal;

    properties->Release();
    device->Release();
}

std::vector<AudioDevice> GetAudioDevices()
{
    IMMDeviceEnumerator* enumerator = nullptr;
    HRESULT status = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), (void**) &enumerator);

    IMMDeviceCollection* collection = nullptr;
    status = enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &collection);

    UINT amount;
    status = collection->GetCount(&amount);

    std::vector<AudioDevice> devices;
    for (UINT i = 0; i < amount; i++)
    {
        AudioDevice audioDevice;
        GetAudioDevice(collection, i, audioDevice);
        devices.push_back(audioDevice);
    }

    collection->Release();
    enumerator->Release();

    return devices;
}