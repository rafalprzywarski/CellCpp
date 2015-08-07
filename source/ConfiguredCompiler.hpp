#ifndef CELL_EB31BDC95E6F4242A7E1AE0003025BFD_HPP
#define CELL_EB31BDC95E6F4242A7E1AE0003025BFD_HPP
#include "Compiler.hpp"
#include "config/config.hpp"
#include "CommandExecutor.hpp"

namespace cell
{

class ConfiguredCompiler : public Compiler
{
public:
    ConfiguredCompiler(CommandExecutorPtr executor, const compiler_desc& desc) : executor(executor), desc(desc) { }
    void compile(const path& cpp, const path& ofile);
    void link(const std::vector<path>& ofiles, const path& target);
    paths get_required_headers(const path& cppfile) override;
private:
    CommandExecutorPtr executor;
    compiler_desc desc;
};

}
#endif // CELL_EB31BDC95E6F4242A7E1AE0003025BFD_HPP
