#pragma once

#include <string>
#include <optional>

class DisplayDevice
{
public:
    DisplayDevice(unsigned int targetId, std::wstring displayName);
    DisplayDevice(unsigned int targetId, std::wstring displayName, unsigned int sourceId);

    unsigned int GetTargetId() const { return m_targetId; };
    const std::wstring& GetDisplayName() const { return m_displayName; }
    std::optional<unsigned int> GetSourceId() const { return m_sourceId; }
private:
    std::wstring m_displayName;
    unsigned int m_targetId;
    std::optional<unsigned int> m_sourceId;
};