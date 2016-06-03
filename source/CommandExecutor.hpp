#ifndef CELL_451565B141EE4427910C9A32AFEB46DC_HPP
#define CELL_451565B141EE4427910C9A32AFEB46DC_HPP
#include <string>
#include <memory>

namespace cell
{

class CommandExecutor
{
public:
    virtual ~CommandExecutor() { }
    virtual void executeCommand(const std::string& command) = 0;
    virtual std::string getCommandOutput(const std::string& command) = 0;
};

}

#endif
