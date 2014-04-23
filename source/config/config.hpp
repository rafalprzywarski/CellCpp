#ifndef CELL_3323DE440C2444C396DE93DD5F7508C3_HPP
#define CELL_3323DE440C2444C396DE93DD5F7508C3_HPP
#include <string>
#include <stdexcept>
#include <fstream>

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

configuration load_configuration();

}
#endif