#include "DisplayDevices.h"

#include <Windows.h>
#include <atlstr.h>
#include <iostream>

// See https://en.wikipedia.org/wiki/Extended_Display_Identification_Data#EDID_1.4_data_format

DisplayDeviceManager::DisplayDeviceManager()
{
    HDEVINFO deviceInfo = SetupDiGetClassDevs(NULL, "Display", NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    
    SP_DEVINFO_DATA deviceInfoData {};
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (size_t index = 0; SetupDiEnumDeviceInfo(deviceInfo, index++, &deviceInfoData);)
    {
        DisplayDevices.push_back(DisplayDevice(deviceInfo, deviceInfoData));
    }

    SetupDiDestroyDeviceInfoList(deviceInfo);
}

DisplayDevice::DisplayDevice(HDEVINFO deviceInfo, SP_DEVINFO_DATA deviceInfoData)
{
    HKEY key = SetupDiOpenDevRegKey(deviceInfo, &deviceInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);

    constexpr size_t VALUE_NAME_LENGTH = 128;
    TCHAR valueName[VALUE_NAME_LENGTH] {};
    DWORD actualValueNameLength = VALUE_NAME_LENGTH;

    DWORD edidSize = sizeof(m_edid);

    LRESULT result = ERROR_SUCCESS;
    DWORD dwType;

    for (size_t index = 0; result != ERROR_NO_MORE_ITEMS; index++)
    {
        result = RegEnumValue(key, index++, &valueName[0], &actualValueNameLength, NULL, &dwType, (BYTE*) m_edid, &edidSize);
        
        if (result == ERROR_SUCCESS)
        {
            if (_tcscmp(valueName, "EDID") != 0) continue;
            
            RegCloseKey(key);
            return;
        }
        else if (result != ERROR_NO_MORE_ITEMS)
        {
            RegCloseKey(key);
            throw std::exception("Failed to retrieve registry key data");
        }
    }
    
    RegCloseKey(key);
    throw std::exception("Failed to retrieve registry key data");
}

const std::string& DisplayDevice::GetDisplayName() const
{
    if (m_displayName) return m_displayName.value();

    constexpr size_t DESCRIPTOR_SIZE = 18;
    constexpr size_t DESCRIPTOR_LOCATIONS[4] = { 54, 72, 90, 108 };

    for (size_t offset : DESCRIPTOR_LOCATIONS)
    {
        const char* descriptor = &m_edid[0] + offset;

        // Display descriptor starts with 00, otherwise it is a detailed timing descriptor
        if (descriptor[0] != 0 || descriptor[1] != 0) continue;
        
        // The display descriptor type has to be the display name type (0xFC)
        unsigned char type = descriptor[3];
        if (type != 0xFC) continue;
        
        const char* name = descriptor + 5;

        // The name is max 13 bytes or early terminated by a LF
        const char* needle = strchr(name, '\n');
        size_t length = needle == NULL ? 13 : needle - name;

        m_displayName = std::string(name, length);
        return m_displayName.value();
    }

    m_displayName = "Name Not Found";
    return m_displayName.value();
}

std::string DisplayDevice::GetManufacturer() const
{   
    // Manufacturer codes are made up of 3 five-bit pairs
    // These values represent 1-indexed letters in the alphabet
    // The five-bit pairs are contained in a 2 big-endian bytes with the highest bit always 0
    char lo = m_edid[8], hi = m_edid[9];
    
    // '@' is the character before 'A' in ASCII
    char first  = '@' + (lo >> 2);
    char second = '@' + (((lo & 0b11) << 3) | ((hi & 0b11100000) >> 5));
    char third  = '@' + (hi & 0b11111);

    return std::string({ first, second, third });
}