#include <iostream>
#include <fstream>
#include <stdexcept>
#include <functional>
#include <boost/filesystem.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/adaptors.hpp>

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

std::vector<boost::filesystem::path> find_cpp_files()
{
    using namespace boost::filesystem;
    using namespace boost::range;
    using namespace boost::adaptors;
    std::function<bool(const directory_entry& e)> is_cpp = [](const directory_entry& e) { return e.path().extension() == ".cpp"; };
    std::function<std::string(const directory_entry& e)> to_string = [](const directory_entry& e) { return e.path().string(); };
    decltype(find_cpp_files()) cpps;
    push_back(cpps, transform(filter(make_iterator_range(directory_iterator(current_path()), directory_iterator()), is_cpp), to_string));
    return cpps;
}

void build_targets(const configuration& configuration, std::function<void(const boost::filesystem::path& )> compile, std::function<void(const std::string& )> link)
{
    for (auto f : find_cpp_files())
        compile(f);
    link(configuration.project_name);
}

void compile_cpp(const boost::filesystem::path& cpp)
{
    auto ofile = cpp;
    ofile.replace_extension(".o");
    if (exists(ofile))
        return;
    auto compile_cmd = "g++ -c " + cpp.string();
    std::system(compile_cmd.c_str());
}

void link_target(const std::string& target)
{
    if (boost::filesystem::exists(target))
        return;
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
