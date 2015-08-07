#ifndef CELL_DA4017C7361D4C838A674207B25FBA1E_HPP
#define CELL_DA4017C7361D4C838A674207B25FBA1E_HPP
#include "Target.hpp"

namespace cell
{
class TargetFactory
{
public:
    TargetFactory(CompilerPtr compiler) : compiler(compiler) { }

    Target createObjectTarget(const path& cpp, const path& obj)
    {
        auto build_command = [=](){ compiler->compile(cpp, obj); };
        if (auto target = Target::load(cpp, obj, build_command))
        {
            return *target;
        }
        auto headers = compiler->get_required_headers(cpp);
        std::vector<Target> deps{headers.begin(), headers.end()};
        deps.push_back(cpp);
        Target target{obj, std::move(deps), build_command};
        target.store(cpp);
        return target;
    }

    Target createExecutableTarget(const paths& objs, const path& exe, std::vector<Target> deps)
    {
        return {exe, std::move(deps), [=](){ compiler->link(objs, exe); }};
    }
private:
    CompilerPtr compiler;
};

}
#endif
