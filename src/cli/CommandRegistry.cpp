#include "CommandRegistry.h"

#include <iostream>
#include <unordered_map>

void CommandRegistry::Register(const Command& command) {
    if (command.name.empty() || !command.action) {
        return;
    }

    if (index_.count(command.name) > 0) {
        std::cerr << "Duplicate command registration skipped: " << command.name << std::endl;
        return;
    }

    index_[command.name] = ordered_.size();
    ordered_.push_back(command);
}

bool CommandRegistry::Exists(const std::string& name) const {
    return index_.count(name) > 0;
}

int CommandRegistry::Run(const std::string& name, const CommandContext& context) const {
    auto it = index_.find(name);
    if (it == index_.end()) {
        std::cerr << "Unknown command: " << name << std::endl;
        return 1;
    }
    return ordered_[it->second].action(context);
}

void CommandRegistry::List(std::ostream& os) const {
    std::unordered_map<std::string, std::vector<const Command*>> grouped;
    for (const auto& cmd : ordered_) {
        grouped[cmd.module].push_back(&cmd);
    }

    for (const auto& [module, commands] : grouped) {
        os << "[" << (module.empty() ? "General" : module) << "]" << std::endl;
        for (const Command* cmd : commands) {
            os << "  - " << cmd->name << ": " << cmd->description << std::endl;
        }
        os << std::endl;
    }
}

std::vector<Command> CommandRegistry::GetCommands() const {
    return ordered_;
}
