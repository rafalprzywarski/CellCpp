#ifndef CELL_3323DE440C2444C396DE93DD5F7508C3_HPP
#define CELL_3323DE440C2444C396DE93DD5F7508C3_HPP
#include "../path.hpp"
#include "property.hpp"
#include <string>
#include <stdexcept>
#include <boost/optional.hpp>

namespace cell
{

struct compiler_desc
{
    std::string executable;
    std::string compile_source;
    std::string link_executable;
    std::string get_used_headers;
};

struct configuration
{
    std::string project_name;
    std::string executable_name;
    std::string output_path;
    compiler_desc compiler;
};

struct configuration_error : std::runtime_error
{
    configuration_error(const std::string& message) : std::runtime_error(message) { }
};

configuration load_configuration();
configuration load_configuration(std::function<config::properties(const path& )> load_file);

}
#endif
