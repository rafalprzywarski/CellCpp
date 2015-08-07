#include "config.hpp"
#include <sstream>
#include <fstream>
#include <map>
#include "parse_properties.hpp"

namespace cell
{

namespace
{
std::string load_text_file(const path& filename)
{
    std::ifstream f(filename.string());
    if (!f.is_open())
        throw configuration_error(filename.string() + " not found");
    return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}

}

configuration load_configuration()
{
    return load_configuration(std::bind(config::parse_properties, std::bind(load_text_file, std::placeholders::_1)));
}

namespace
{

std::map<std::string, std::string> map_properties(const config::properties& properties)
{
    decltype(map_properties(properties)) mapped;
    for (auto& p : properties)
        mapped[p.name] = p.value;
    return mapped;
}

template <typename Map>
const typename Map::mapped_type& get_value_or(const Map& m, const typename Map::key_type& k, const typename Map::mapped_type& v)
{
    auto it = m.find(k);
    return it != m.end() ? it->second : v;
}

compiler_desc unpack_compiler_properties(const config::properties& properties)
{
    auto mapped = map_properties(properties);
    compiler_desc compiler;
    compiler.executable = mapped["executable"];
    compiler.compile_source = mapped["compile-source"];
    compiler.link_executable = mapped["link-executable"];
    compiler.get_used_headers = mapped["get-used-headers"];
    return compiler;
}

compiler_desc load_compiler_configuration(const std::string& name, std::function<config::properties(const path& )> load_file)
{
    return unpack_compiler_properties(load_file(name + ".cell"));
}

compiler_desc get_default_compiler_configuration()
{
    compiler_desc compiler;
    compiler.executable = "g++";
    compiler.compile_source = "-c $(SOURCE) -o $(OBJECT)";
    compiler.link_executable = "$(OBJECTS) -o $(EXECUTABLE)";
    compiler.get_used_headers = "-MM $(SOURCE)";
    return compiler;
}

configuration unpack_properties(const config::properties& properties, std::function<config::properties(const path& )> load_file)
{
    auto mapped = map_properties(properties);
    configuration configuration;
    configuration.project_name = mapped["project"];
    configuration.executable_name = get_value_or(mapped, "executable", configuration.project_name);
    configuration.compiler = mapped.count("compiler") ? load_compiler_configuration(mapped["compiler"], load_file) : get_default_compiler_configuration();
    return configuration;
}

}

configuration load_configuration(std::function<config::properties(const path& )> load_file)
{
    return unpack_properties(load_file("build.cell"), load_file);
}

}
