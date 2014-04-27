#include "config.hpp"
#include <sstream>
#include <fstream>
#include <boost/spirit/home/qi.hpp>

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

namespace
{

namespace qi = ::boost::spirit::qi;
namespace ascii = ::boost::spirit::ascii;

template <typename Iterator>
struct config_grammar : boost::spirit::qi::grammar<Iterator, std::string(), ascii::space_type>
{
    config_grammar()
        : config_grammar::base_type(grammar, "config-grammar")
    {
    }

    qi::rule<Iterator, std::string(), ascii::space_type> identifier{qi::lexeme[+qi::char_("a-zA-Z0-9_")]};
    qi::rule<Iterator, std::string(), ascii::space_type> grammar{qi::lit("project") >> qi::lit(":") > identifier};
};

}

configuration load_configuration(std::function<std::string(const boost::filesystem::path& )> load_file)
{
    std::string project(load_file("build.cell"));

    auto first = begin(project);
    config_grammar<std::string::iterator> grammar;
    configuration configuration;
    boost::spirit::qi::phrase_parse(first, end(project), grammar, ascii::space, configuration.project_name);

    return std::move(configuration);
}

}
