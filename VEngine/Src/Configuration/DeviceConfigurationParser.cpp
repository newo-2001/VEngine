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

AudioDevice ParseAudioDevice(const std::string& device)
{
    std::smatch match;
    std::regex_match(device, match, std::regex("^(.+?), ?(.+)$"));
    if (match.size() != 3) throw std::exception("Expected audio device");

    std::wstring name = StringToWString(match[1]);
    std::wstring id = StringToWString(match[2]);
    
    return AudioDevice(name, id);
}

std::optional<AudioConfiguration> ParseAudioBlock(std::istringstream& ss)
{
    std::optional<AudioDevice> primaryOutputDevice;

    std::string line;
    while (std::getline(ss, line))
    {
        if (line == "[/Audio]")
        {
            return primaryOutputDevice.has_value()
                ? std::make_optional(AudioConfiguration(primaryOutputDevice.value()))
                : std::nullopt;
        } else if (line.starts_with("Primary Output Device = "))
        {
            if (primaryOutputDevice.has_value())
                throw std::exception("Primary Output Device declared more than once");

            primaryOutputDevice = ParseAudioDevice(line.substr(sizeof("Primary OutputDevice = ")));
        }
    }

    throw std::exception("Expected end of audio block");
}

void ParseConfigurationBlock(DeviceConfiguration& configuration, std::istringstream& ss)
{
    std::string line;

    // Read configuration name
    std::getline(ss, line);
    std::smatch match;
    std::regex_match(line, match, std::regex("^<([^<>]+)>$"));

    if (match.size() != 2) throw std::exception("Expected start of a new configuration block");
    configuration.Name = match[1];

    while (std::getline(ss, line))
    {
        if (line == "[Displays]")
        {
            configuration.Displays = ParseDisplaysBlock(ss);
        } else if (line == "[Audio]")
        {
            configuration.Audio = ParseAudioBlock(ss);
        } else if (line == std::format("</{}>", configuration.Name))
        {
            return;
        } else
        {
            throw std::exception("Expected displays block, audio block or end of configuration block");
        }
    }

    throw std::exception("Expected end of configuration block");
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
        DeviceConfiguration configuration;
        ParseConfigurationBlock(configuration, ss);
        configurations.insert({ configuration.Name, configuration });
    }
    
    return configurations;
}
