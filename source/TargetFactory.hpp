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
        Target d{dependencyListName(cpp), {cpp}, [=]
        {
            auto headers = compiler->get_required_headers(cpp);
            paths deps{headers.begin(), headers.end()};
            deps.push_back(cpp);
            storeObjectDependencies(cpp, deps);
        }};
        d.build();

        auto build_command = [=](){ compiler->compile(cpp, obj); };
        return *loadObjectTarget(cpp, obj, build_command);
    }

    Target createExecutableTarget(const paths& objs, const path& exe, std::vector<Target> deps)
    {
        return {exe, std::move(deps), [=](){ compiler->link(objs, exe); }};
    }
private:
    CompilerPtr compiler;

    void storeObjectDependencies(path cpp, const paths& deps)
    {
        std::ofstream f(dependencyListName(cpp));
        for (auto& dep : deps)
          f << dep << '\n';
    }

    static boost::optional<Target> loadObjectTarget(path cpp, path obj, fn<void()> build_command)
    {
        std::ifstream f(dependencyListName(cpp));
        if (!f.is_open())
          return {};
        std::vector<Target> dependencies{std::istream_iterator<path>(f), std::istream_iterator<path>()};
        return Target{obj, dependencies, build_command};
    }

    static std::string dependencyListName(const path& cpp)
    {
        return cpp.string() + ".d";
    }
};

}
#endif
