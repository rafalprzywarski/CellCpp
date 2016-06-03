#include "config/config.hpp"
#include "find_cpp_files.hpp"
#include "ConfiguredCompiler.hpp"
#include <boost/filesystem.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <functional>
#include <iostream>
#include "TargetFactory.hpp"
#include "NativeCommandExecutor.hpp"

namespace cell
{

class ConfigurationProvider
{
public:
    ConfigurationProvider() : config(load_configuration()) { }
    path getExecutablePath() const { return getOutputPath() / path(config.executable_name); }
    paths getCppFiles() const { return find_cpp_files(); }
    p<Compiler> getCompiler() const { return std::make_shared<ConfiguredCompiler>(std::make_shared<NativeCommandExecutor>(), config.compiler); }
    path getObjectFilePath(const path& cpp) const { return (getOutputPath() / cpp).string() + ".o"; }
    path getOutputPath() const { return config.output_path; }
    path getDependencyListPath(const path& cpp) const { return (getOutputPath() / cpp).string() + ".d"; }
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
            auto ofile = configProvider.getObjectFilePath(cpp.string());
            ofiles.push_back(ofile);
            deps.push_back(targetFactory.createObjectTarget(cpp, ofile));
        }
        auto tgt = targetFactory.createExecutableTarget(ofiles, configProvider.getExecutablePath(), deps);
        create_directories(configProvider.getOutputPath());
        tgt.build();
    }
private:
    ConfigurationProvider& configProvider;
    TargetFactory& targetFactory;
};

void run()
{
    ConfigurationProvider configProvider;
    TargetFactory targetFactory{configProvider.getCompiler(), [&](const path& cpp) { return configProvider.getDependencyListPath(cpp); }};
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
