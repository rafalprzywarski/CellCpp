#include "config.hpp"
#include <sstream>
#include <fstream>
#include <map>
#include <boost/spirit/home/qi.hpp>
#include "property.hpp"

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
    return load_configuration(load_text_file);
}

namespace
{

namespace qi = ::boost::spirit::qi;
namespace ascii = ::boost::spirit::ascii;

template <typename Iterator>
struct config_grammar : boost::spirit::qi::grammar<Iterator, config::properties(), ascii::space_type>
{
    config_grammar()
        : config_grammar::base_type(grammar, "config-grammar")
    {
    }

    qi::rule<Iterator, std::string(), ascii::space_type> identifier{qi::lexeme[+qi::char_("a-zA-Z0-9_\\-")]};
    qi::rule<Iterator, std::string(), ascii::space_type> string{qi::lexeme['\'' > *(qi::char_ - '\'') > '\'']};
    qi::rule<Iterator, config::property(), ascii::space_type> property{identifier > ":" >> (identifier | string)};
    qi::rule<Iterator, config::properties(), ascii::space_type> grammar{*property};
};

std::map<std::string, std::string> map_properties(const config::properties& properties)
{
    decltype(map_properties(properties)) mapped;
    for (auto& p : properties)
        mapped[p.name] = p.value;
    return mapped;
}

config::properties parse_properties(const std::string& text)
{
    auto first = begin(text);
    config_grammar<std::string::const_iterator> grammar;
    config::properties properties;
    boost::spirit::qi::phrase_parse(first, end(text), grammar, ascii::space, properties);

    return properties;
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
    return compiler;
}

compiler_desc load_compiler_configuration(const std::string& name, std::function<std::string(const path& )> load_file)
{
    return unpack_compiler_properties(parse_properties(load_file(name + ".cell")));
}

compiler_desc get_default_compiler_configuration()
{
    compiler_desc compiler;
    compiler.executable = "g++";
    compiler.compile_source = "-c $(SOURCE) -o $(OBJECT)";
    compiler.link_executable = "$(OBJECTS) -o $(EXECUTABLE)";
    return compiler;
}

configuration unpack_properties(const config::properties& properties, std::function<std::string(const path& )> load_file)
{
    auto mapped = map_properties(properties);
    configuration configuration;
    configuration.project_name = mapped["project"];
    configuration.executable_name = get_value_or(mapped, "executable", configuration.project_name);
    configuration.compiler = mapped.count("compiler") ? load_compiler_configuration(mapped["compiler"], load_file) : get_default_compiler_configuration();
    return configuration;
}

}

configuration load_configuration(std::function<std::string(const path& )> load_file)
{
    return unpack_properties(parse_properties(load_file("build.cell")), load_file);
}

}
