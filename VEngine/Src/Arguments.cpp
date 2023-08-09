#include "Arguments.h"
#include <iostream>
#include <vector>
#include <format>

Arguments::Arguments(int argc, char** argv)
{
    for (size_t i = 1; i < argc; i++)
    {
        std::string token = argv[i];

        if (token == "-f")
        {
            if (++i >= argc) throw std::exception("Expected file path as argument");
            std::string path = argv[i];

            ConfigurationFilePath = path;
        }
        else if (token == "-c")
        {
            if (i++ >= argc) throw std::exception("Expected configuration name as argument");
            std::string configuration = argv[i];
            
            Configuration = configuration;
            Action = Action::ApplyConfiguration;
        }
        else if (token == "-l")
        {
            Action = Action::ListActiveDevices;
        }
        else if (token == "--help")
        {
            Action = Action::ShowHelp;
        }
        else
        {
            throw std::exception(std::format("Unexpected argument {}", token).c_str());
        }
    }
}