#include "config/config.hpp"
#include "find_cpp_files.hpp"
#include "ConfiguredCompiler.hpp"
#include <boost/filesystem.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <functional>

namespace cell
{

template <typename F>
using fn = std::function<F>;

struct Target
{
    void add_dependency(Target dep)
    {
        dependencies.push_back(std::move(dep));
    }

    void set_dependencies(std::vector<Target> deps)
    {
        dependencies = std::move(deps);
    }

    void build()
    {
        for (auto& dep : dependencies)
            dep.build();
        if (is_out_of_date())
            build_command();
    }

    void set_build_command(fn<void()> cmd)
    {
        build_command = std::move(cmd);
    }

    Target(path file) : file(std::move(file)) { }
private:
    path file;
    std::vector<Target> dependencies;
    fn<void()> build_command;

    bool is_out_of_date() const
    {
        if (!exists(file))
            return true;
        auto modification_time = get_modification_time();
        return boost::find_if(dependencies, [=](const Target& dep) { return dep.get_modification_time() > modification_time; }) != dependencies.end();
    }

    std::time_t get_modification_time() const
    {
        return last_write_time(file);
    }
};

Target create_object_target(const path& cpp, const path& obj, fn<void(const path& , const path& )> compile)
{
    Target target{obj};
    target.set_build_command([=](){ compile(cpp, obj); });
    target.add_dependency(cpp);
    return target;
}

Target create_executable_target(const paths& objs, const path& exe, std::vector<Target> deps, fn<void(const paths& , const path& )> link)
{
    Target target{exe};
    target.set_build_command([=](){ link(objs, exe); });
    target.set_dependencies(std::move(deps));
    return target;
}

void build_targets(const std::string& executable_name, const paths& cpps, fn<Target(const path& , const path& )> create_object_target, fn<void(const std::vector<path>& , const path& )> link)
{
    std::vector<path> ofiles;
    std::vector<Target> deps;
    for (auto cpp : cpps)
    {
        auto ofile = cpp.string() + ".o";
        ofiles.push_back(ofile);
        deps.push_back(create_object_target(cpp, ofile));
    }
    auto tgt = create_executable_target(ofiles, executable_name, deps, link);
    tgt.build();
}

void build_targets(Compiler& compiler, const std::string& executable_name, const paths& cpps)
{
    using namespace std::placeholders;
    using std::bind;
    fn<void(const path& , const path& )> compile = bind(&Compiler::compile, &compiler, _1, _2);
    build_targets(
        executable_name,
        cpps,
        bind(create_object_target, _1, _2, compile),
        bind(&Compiler::link, &compiler, _1, _2));
}

void run()
{
    auto configuration = load_configuration();
    auto cpps = find_cpp_files();
    ConfiguredCompiler compiler(configuration.compiler);
    build_targets(compiler, configuration.executable_name, cpps);
}

}

int main()
{
    try
    {
        cell::run();
    }
    catch (const cell::configuration_error& error)
    {
        std::cerr << error.what() << std::endl;
        return 1;
    }
    return 0;
}
