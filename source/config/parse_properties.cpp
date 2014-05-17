#include "parse_properties.hpp"
#include <boost/spirit/home/qi.hpp>

namespace cell
{
namespace config
{

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

}

properties parse_properties(const std::string& text)
{
    auto first = begin(text);
    config_grammar<std::string::const_iterator> grammar;
    config::properties properties;
    boost::spirit::qi::phrase_parse(first, end(text), grammar, ascii::space, properties);

    return properties;
}

}
}
