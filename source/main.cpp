#include "config/config.hpp"
#include "find_cpp_files.hpp"
#include "ConfiguredCompiler.hpp"
#include <boost/filesystem.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <functional>
#include <iostream>
#include "TargetFactory.hpp"

namespace cell
{

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
