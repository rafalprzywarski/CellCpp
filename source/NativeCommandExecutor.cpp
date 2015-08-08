#include "NativeCommandExecutor.hpp"
#include <cstdlib>
#include <cstring>

namespace cell
{

void NativeCommandExecutor::executeCommand(const std::string& command)
{
    std::system(command.c_str());
}

std::string NativeCommandExecutor::getCommandOutput(const std::string& command)
{
    auto pipe = popen(command.c_str(), "r");
    char buffer[1024];
    std::fgets(buffer, sizeof(buffer), pipe);
    pclose(pipe);
    return {buffer, std::strlen(buffer) - 1};
}

}
