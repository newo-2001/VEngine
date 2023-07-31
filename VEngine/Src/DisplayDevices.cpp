#include "DisplayDevices.h"

#include <Windows.h>
#include <SetupAPI.h>
#include <atlstr.h>
#include <iostream>

// See https://en.wikipedia.org/wiki/Extended_Display_Identification_Data#EDID_1.4_data_format

// Manufacturer codes are made up of 3 five-bit pairs
// These values represent 1-indexed letters in the alphabet
// The five-bit pairs are contained in a 2 big-endian bytes with the highest bit always 0
std::string DecodeManufacturerCode(const char* data, size_t length)
{
    char lo = data[8], hi = data[9];
    
    // '@' is the character before 'A' in ASCII
    char first  = '@' + (lo >> 2);
    char second = '@' + (((lo & 0b11) << 3) | ((hi & 0b11100000) >> 5));
    char third  = '@' + (hi & 0b11111);

    return std::string({ first, second, third });
}

std::string DecodeDisplayName(const char* data, size_t length)
{
    constexpr size_t DESCRIPTOR_SIZE = 18;
    constexpr size_t DESCRIPTOR_LOCATIONS[4] = { 54, 72, 90, 108 };

    for (size_t offset : DESCRIPTOR_LOCATIONS)
    {
        const char* descriptor = data + offset;

        // Display descriptor starts with 00, otherwise it is a detailed timing descriptor
        if (descriptor[0] != 0 || descriptor[1] != 0) continue;
        
        // The display descriptor type has to be the display name type (0xFC)
        unsigned char type = descriptor[3];
        if (type != 0xFC) continue;
        
        const char* name = descriptor + 5;

        // The name is max 13 bytes or early terminated by a LF
        const char* needle = strchr(name, '\n');
        size_t length = needle == NULL ? 13 : needle - name;

        return std::string(name, length);
    }

    return "Name Not Found";
}

void DecodeEdid(const char* edid, size_t length, DisplayDevice& device)
{
    device.Manufacturer = DecodeManufacturerCode(edid, length);
    device.DisplayName  = DecodeDisplayName(edid, length);
}

bool ExtractDisplayDeviceFromRegistryKey(HKEY key, DisplayDevice& device)
{
    constexpr size_t VALUE_NAME_LENGTH = 128;
    TCHAR valueName[VALUE_NAME_LENGTH] {};
    DWORD actualValueNameLength = VALUE_NAME_LENGTH;

    BYTE edidData[1024];
    DWORD edidSize = sizeof(edidData);

    LRESULT result = ERROR_SUCCESS;
    DWORD dwType;

    for (size_t index = 0; result != ERROR_NO_MORE_ITEMS; index++)
    {
        result = RegEnumValue(key, index++, &valueName[0], &actualValueNameLength, NULL, &dwType, edidData, &edidSize);
        
        if (result == ERROR_SUCCESS)
        {
            if (_tcscmp(valueName, L"EDID") != 0) continue;

            DecodeEdid((char*) edidData, edidSize, device);
            return true;
        }
        else if (result != ERROR_NO_MORE_ITEMS)
        {
            std::cerr << "[Error] Failed to retrieve registry key data, error code: " << std::to_string(result) << std::endl;
            return false;
        }
    }
    
    std::cerr << "[Error] Failed to resolve EDID registry key" << std::endl;
    return false;
}

std::vector<DisplayDevice> GetDisplayDevices()
{
    HDEVINFO info = SetupDiGetClassDevs(NULL, L"Display", NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    
    SP_DEVINFO_DATA deviceInfo {};
    deviceInfo.cbSize = sizeof(SP_DEVINFO_DATA);

    std::vector<DisplayDevice> devices;
    for (size_t index = 0; SetupDiEnumDeviceInfo(info, index++, &deviceInfo);)
    {
        HKEY key = SetupDiOpenDevRegKey(info, &deviceInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
        
        DisplayDevice device;
        if (ExtractDisplayDeviceFromRegistryKey(key, device))
        {
            devices.push_back(device);
        }

        RegCloseKey(key);
    }

    SetupDiDestroyDeviceInfoList(info);

    return devices;
}