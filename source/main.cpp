#include <iostream>
#include <fstream>
#include <stdexcept>
#include <functional>

namespace cell
{

struct configuration
{
    std::string project_name;
};

struct configuration_error : std::runtime_error
{
    configuration_error(const std::string& message) : std::runtime_error(message) { }
};

configuration load_configuration()
{
    std::ifstream project("build.cell");
    if (!project.is_open())
        throw configuration_error("build.cell not found");

    std::string dummy;
    configuration configuration;
    project >> dummy >> configuration.project_name;
    return std::move(configuration);
}

void build_targets(const configuration& configuration, std::function<void(const std::string& )> compile, std::function<void(const std::string& )> link)
{
    compile("*.cpp");
    link(configuration.project_name);
}

void compile_cpp(const std::string& cpp)
{
    auto compile_cmd = "g++ -c " + cpp;
    std::system(compile_cmd.c_str());
}

void link_target(const std::string& target)
{
    auto link_cmd = "g++ *.o -o " + target;
    std::system(link_cmd.c_str());
}

void build_targets(const configuration& configuration)
{
    build_targets(configuration, compile_cpp, link_target);
}

void run()
{
    auto configuration = load_configuration();
    build_targets(configuration);
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
