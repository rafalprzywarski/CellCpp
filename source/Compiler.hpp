#ifndef CELL_ED9135CE49D14C849709DFE2D2D7D2FB_HPP
#define CELL_ED9135CE49D14C849709DFE2D2D7D2FB_HPP
#include "path.hpp"

namespace cell
{

class Compiler
{
public:
    virtual ~Compiler() { }
    virtual void compile(const path& cppfile, const path& ofile) = 0;
    virtual void link(const paths& ofiles, const path& executable) = 0;
};

}
#endif // CELL_ED9135CE49D14C849709DFE2D2D7D2FB_HPP
