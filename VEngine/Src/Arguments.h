#pragma once
#include <string>
#include <optional>

enum class Action
{
    ShowHelp,
    ListActiveDevices,
    ApplyConfiguration
};

struct Arguments
{
public:
    Arguments(int argc, char** argv);

    std::string ConfigurationFilePath;
    std::optional<std::string> Configuration;
    Action Action;
};
