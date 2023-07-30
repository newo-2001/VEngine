#pragma comment (lib, "Setupapi.lib")

#include <iostream>
#include <vector>
#include <Windows.h>
#include <SetupAPI.h>
#include <atlstr.h>

#include "Edid.h"

bool ExtractEdidFromRegistryKey(HKEY key, Edid& edid)
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

            DecodeEdid((char*) edidData, edidSize, edid);
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

std::vector<Edid> GetDisplayEdids()
{
    HDEVINFO info = SetupDiGetClassDevs(NULL, L"Display", NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    
    SP_DEVINFO_DATA deviceInfo {};
    deviceInfo.cbSize = sizeof(SP_DEVINFO_DATA);

    std::vector<Edid> entries;
    for (size_t index = 0; SetupDiEnumDeviceInfo(info, index++, &deviceInfo);)
    {
        HKEY key = SetupDiOpenDevRegKey(info, &deviceInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
        
        Edid edid;
        if (ExtractEdidFromRegistryKey(key, edid))
        {
            entries.push_back(edid);
        }

        RegCloseKey(key);
    }

    SetupDiDestroyDeviceInfoList(info);

    return entries;
}

int main()
{
    std::cout << "Detected display devices:" << std::endl;

    std::vector<Edid> edids = GetDisplayEdids();
    for (Edid &edid : edids)
    {
        std::cout << '\t' << edid.Manufacturer << ": " << edid.DisplayName << std::endl;
    }

    return 0;
}
