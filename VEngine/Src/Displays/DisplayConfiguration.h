#pragma once
#include <vector>
#include <string>
#include "DisplayDevice.h"

class DisplayConfiguration
{
public:
    DisplayConfiguration(std::vector<DisplayDevice> enabledDisplays);

    static DisplayConfiguration Parse(std::string config);
    static DisplayConfiguration Active();

    const DisplayDevice& FindDisplayByTargetId(unsigned int targetId) const;
    const DisplayDevice& FindDisplayByName(std::wstring displayName) const;

    void Apply();

    std::vector<DisplayDevice> EnabledDisplays;
};
