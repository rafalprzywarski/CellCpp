#include "config/config.hpp"
#include "find_cpp_files.hpp"
#include "ConfiguredCompiler.hpp"
#include <boost/filesystem.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <functional>
#include <iostream>

namespace cell
{

template <typename F>
using fn = std::function<F>;

struct Target
{
    void build()
    {
        for (auto& dep : dependencies)
            dep.build();
        if (is_out_of_date())
            build_command();
    }

    Target(path file) : file(std::move(file)) { }

    Target(path file, std::vector<Target> dependencies, fn<void()> build_command) : file(std::move(file)), dependencies(std::move(dependencies)), build_command(build_command) { }
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

class TargetFactory
{
public:
    TargetFactory(CompilerPtr compiler) : compiler(compiler) { }

    Target createObjectTarget(const path& cpp, const path& obj)
    {
        auto headers = compiler->get_required_headers(cpp);
        std::vector<Target> deps{headers.begin(), headers.end()};
        deps.push_back(cpp);
        return {obj, std::move(deps), [=](){ compiler->compile(cpp, obj); }};
    }

    Target createExecutableTarget(const paths& objs, const path& exe, std::vector<Target> deps)
    {
        return {exe, std::move(deps), [=](){ compiler->link(objs, exe); }};
    }
private:
    CompilerPtr compiler;
};

class ConfigurationProvider
{
public:
    ConfigurationProvider() : config(load_configuration()) { }
    std::string getExecutableName() const { return config.executable_name; }
    paths getCppFiles() const { return find_cpp_files(); }
    CompilerPtr getCompiler() const { return std::make_shared<ConfiguredCompiler>(config.compiler); }
private:
    configuration config;
};

class CellRunner
{
public:
    CellRunner(ConfigurationProvider& configProvider, TargetFactory& targetFactory)
        : configProvider(configProvider), targetFactory(targetFactory) { }
    void buildProject()
    {
        std::vector<path> ofiles;
        std::vector<Target> deps;
        for (auto cpp : configProvider.getCppFiles())
        {
            auto ofile = cpp.string() + ".o";
            ofiles.push_back(ofile);
            deps.push_back(targetFactory.createObjectTarget(cpp, ofile));
        }
        auto tgt = targetFactory.createExecutableTarget(ofiles, configProvider.getExecutableName(), deps);
        tgt.build();
    }
private:
    ConfigurationProvider& configProvider;
    TargetFactory& targetFactory;
};

void run()
{
    ConfigurationProvider configProvider;
    TargetFactory targetFactory{configProvider.getCompiler()};
    CellRunner runner{configProvider, targetFactory};
    runner.buildProject();
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
