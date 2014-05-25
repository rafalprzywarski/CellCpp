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
private:
    compiler_desc desc;
};

}
#endif // CELL_EB31BDC95E6F4242A7E1AE0003025BFD_HPP
