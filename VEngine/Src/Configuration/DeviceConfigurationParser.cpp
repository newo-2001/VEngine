#include "DeviceConfigurationParser.h"
#include <sstream>
#include <format>
#include <regex>
#include <algorithm>
#include "../Utils.h"

std::optional<DisplayDevice> ParseDisplay(const std::string& line)
{
    std::smatch match;
    std::regex_match(line, match, std::regex("^(.+?), ?(\\d+), ?(\\d+)$"));
    if (match.size() != 4) return std::nullopt;

    unsigned int targetId = std::stoi(match[2].str());
    unsigned int sourceId = std::stoi(match[3].str());
    std::wstring displayName = StringToWString(match[1]);
    return DisplayDevice(targetId, displayName, sourceId);
}

DisplayConfiguration ParseDisplaysBlock(std::istringstream& ss)
{
    DisplayConfiguration displayConfiguration;

    std::string line;
    while (std::getline(ss, line))
    {
        if (line == "[/Displays]") return displayConfiguration;

        std::optional<DisplayDevice> display = ParseDisplay(line);
        if (!display) throw std::exception("Failed to parse display device");

        displayConfiguration.EnabledDisplays.push_back(display.value());
    }

    throw std::exception("Expected end of displays block");
}

DeviceConfiguration ParseConfigurationBlock(std::istringstream& ss)
{
    std::string line;

    // Read configuration name
    std::getline(ss, line);
    std::smatch match;
    std::regex_match(line, match, std::regex("^<([^<>]+)>$"));

    if (match.size() != 2) throw std::exception("Expected start of a new configuration block");
    std::string name = match[1];

    DisplayConfiguration displayConfiguration;

    while (std::getline(ss, line))
    {
        if (line == "[Displays]")
        {
            displayConfiguration = ParseDisplaysBlock(ss);
        } else if (line == std::format("</{}>", name))
        {
            break;
        } else
        {
            throw std::exception("Expected displays block or end of configuration block");
        }
    }

    return DeviceConfiguration(name, displayConfiguration);
}

void StripBlankLines(std::string& data)
{
    // Replace CRLF with LF
    size_t pos = 0;
    while ((pos = data.find("\r\n", pos)) != std::string::npos)
    {
        data.erase(pos, 1);
    }


    // Remove blank lines
    pos = 0;
    while ((pos = data.find("\n\n", pos)) != std::string::npos)
    {
        data.erase(pos, 1);
    }
}

DeviceConfigurationMap DeviceConfigurationParser::Parse(const std::string& data) const
{
    std::string stripped = data;
    StripBlankLines(stripped);

    std::istringstream ss(stripped);
    DeviceConfigurationMap configurations;

    while (!ss.eof())
    {
        DeviceConfiguration configuration = ParseConfigurationBlock(ss);
        configurations.insert({ configuration.GetName(), configuration });
    }
    
    return configurations;
}
