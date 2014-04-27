#include "config.hpp"
#include <sstream>
#include <fstream>

namespace cell
{

namespace
{
std::string load_text_file(const boost::filesystem::path& filename)
{
    std::ifstream f(filename.string());
    if (!f.is_open())
        throw configuration_error(filename.string() + " not found");
    return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}

}

configuration load_configuration()
{
    return load_configuration(load_text_file);
}

configuration load_configuration(std::function<std::string(const boost::filesystem::path& )> load_file)
{
    std::istringstream project(load_file("build.cell"));
    std::string dummy;
    configuration configuration;
    project >> dummy >> configuration.project_name;
    return std::move(configuration);
}

}
