#include "DisplayDevice.h"
#include <Windows.h>
#include <iostream>

DisplayDevice::DisplayDevice(unsigned int targetId, std::wstring displayName)
    : m_targetId(targetId), m_displayName(displayName)
{ }

DisplayDevice::DisplayDevice(unsigned int targetId, std::wstring displayName, unsigned int sourceId)
    : m_targetId(targetId), m_displayName(displayName), m_sourceId(sourceId)
{ }
