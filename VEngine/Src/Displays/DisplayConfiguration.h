#pragma once
#include <vector>
#include <string>
#include "DisplayDevice.h"

class DisplayConfiguration
{
public:
    DisplayConfiguration();
    DisplayConfiguration(std::vector<DisplayDevice> enabledDisplays);

    static DisplayConfiguration Active();

    const DisplayDevice& FindDisplayByTargetId(unsigned int targetId) const;
    const DisplayDevice& FindDisplayByName(std::wstring displayName) const;

    void Apply();
    void Print() const;

    std::vector<DisplayDevice> EnabledDisplays;
};
