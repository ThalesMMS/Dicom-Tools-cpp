#pragma once

#include <string>

struct CLIOptions {
    std::string command;
    std::string inputPath;
    std::string outputDir{"output"};
    bool list{false};
    bool modules{false};
    bool help{false};
    bool verbose{false};
};
