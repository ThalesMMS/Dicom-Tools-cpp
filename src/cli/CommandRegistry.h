#pragma once

#include <functional>
#include <map>
#include <ostream>
#include <string>
#include <vector>

struct CommandContext {
    std::string inputPath;
    std::string outputDir;
    bool verbose{false};
};

struct Command {
    std::string name;
    std::string module;
    std::string description;
    std::function<int(const CommandContext&)> action;
};

class CommandRegistry {
public:
    void Register(const Command& command);
    bool Exists(const std::string& name) const;
    int Run(const std::string& name, const CommandContext& context) const;
    void List(std::ostream& os) const;
    std::vector<Command> GetCommands() const;

private:
    std::vector<Command> ordered_;
    std::map<std::string, std::size_t> index_;
};
