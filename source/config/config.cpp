#include "config.hpp"

namespace cell
{

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

}
