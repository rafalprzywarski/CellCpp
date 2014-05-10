#ifndef CELL_3323DE440C2444C396DE93DD5F7508C3_HPP
#define CELL_3323DE440C2444C396DE93DD5F7508C3_HPP
#include "../path.hpp"
#include <string>
#include <stdexcept>
#include <boost/optional.hpp>

namespace cell
{

struct configuration
{
    std::string project_name;
    std::string executable_name;
    std::string compiler_name;
};

struct configuration_error : std::runtime_error
{
    configuration_error(const std::string& message) : std::runtime_error(message) { }
};

configuration load_configuration();
configuration load_configuration(std::function<std::string(const path& )> load_file);

}
#endif
