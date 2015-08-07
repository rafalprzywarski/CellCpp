#ifndef CELL_C45BAC76087D4269BE8D40BDCE7E39AC_HPP
#define CELL_C45BAC76087D4269BE8D40BDCE7E39AC_HPP
#include "CommandExecutor.hpp"

namespace cell
{

class NativeCommandExecutor : public CommandExecutor
{
public:
    void executeCommand(const std::string& command) override;
    std::string getCommandOutput(const std::string& command) override;
};

}

#endif
