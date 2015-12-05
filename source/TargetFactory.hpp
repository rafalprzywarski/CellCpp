#ifndef CELL_DA4017C7361D4C838A674207B25FBA1E_HPP
#define CELL_DA4017C7361D4C838A674207B25FBA1E_HPP
#include "Target.hpp"

namespace cell
{
class TargetFactory
{
public:
    TargetFactory(CompilerPtr compiler, fn<path(const path& )> getDependencyListPath) : compiler(compiler), getDependencyListPath(getDependencyListPath) { }

    Target createObjectTarget(const path& cpp, const path& obj)
    {
        auto build_command = [=]()
        {
            auto headers = compiler->get_required_headers(cpp);
            paths deps{headers.begin(), headers.end()};
            deps.push_back(cpp);
            storeObjectDependencies(cpp, deps);
            compiler->compile(cpp, obj);
        };
        return Target{obj, loadDependencies(cpp), build_command};
    }

    Target createExecutableTarget(const paths& objs, const path& exe, std::vector<Target> deps)
    {
        return {exe, std::move(deps), [=](){ compiler->link(objs, exe); }};
    }
private:
    CompilerPtr compiler;
    fn<path(const path& )> getDependencyListPath;

    void storeObjectDependencies(path cpp, const paths& deps)
    {
        std::ofstream f(getDependencyListPath(cpp).string());
        for (auto& dep : deps)
          f << dep << '\n';
    }

    std::vector<Target> loadDependencies(path cpp)
    {
        std::ifstream f(getDependencyListPath(cpp).string());
        if (!f.is_open())
          return {cpp};
        return {std::istream_iterator<path>(f), std::istream_iterator<path>()};
    }
};

}
#endif
