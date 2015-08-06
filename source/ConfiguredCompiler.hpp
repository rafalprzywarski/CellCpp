#ifndef CELL_EB31BDC95E6F4242A7E1AE0003025BFD_HPP
#define CELL_EB31BDC95E6F4242A7E1AE0003025BFD_HPP
#include "Compiler.hpp"
#include "config/config.hpp"

namespace cell
{

class ConfiguredCompiler : public Compiler
{
public:
    ConfiguredCompiler(const compiler_desc& desc) : desc(desc) { }
    void compile(const path& cpp, const path& ofile);
    void link(const std::vector<path>& ofiles, const path& target);
    paths get_required_headers(const path& cppfile) override;
private:
    compiler_desc desc;
    std::string get_command_output(const std::string& cmd);
};

}
#endif // CELL_EB31BDC95E6F4242A7E1AE0003025BFD_HPP
