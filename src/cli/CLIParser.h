#pragma once

#include <ostream>

#include "CLIOptions.h"
#include "CommandRegistry.h"

CLIOptions ParseCLIArgs(int argc, char* argv[], const CommandRegistry& registry);
void PrintUsage(std::ostream& os, const CommandRegistry& registry);
