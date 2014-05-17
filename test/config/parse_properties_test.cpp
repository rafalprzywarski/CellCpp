#include <config/parse_properties.hpp>
#include <gtest/gtest.h>

namespace cell
{
namespace config
{

TEST(parse_properties_test, should_parse_properties_separated_by_new_line_and_names_separated_from_values_by_colons)
{
    properties expected = { { "First", "55" }, { "Second", "false" }};
    auto parsed = parse_properties("First: 55\nSecond: false");
    ASSERT_EQ(2u, parsed.size());
    ASSERT_EQ("First", parsed[0].name);
    ASSERT_EQ("55", parsed[0].value);
    ASSERT_EQ("Second", parsed[1].name);
    ASSERT_EQ("false", parsed[1].value);
}

TEST(parse_properties_test, should_allow_minus_signs_in_property_names_and_values)
{
    auto parsed = parse_properties("-f-a-: -c-d-");
    ASSERT_EQ("-f-a-", parsed.at(0).name);
    ASSERT_EQ("-c-d-", parsed.at(0).value);
}

TEST(parse_properties_test, should_allow_underscores_in_property_names_and_values)
{
    auto parsed = parse_properties("_f_a_: _c_d_");
    ASSERT_EQ("_f_a_", parsed.at(0).name);
    ASSERT_EQ("_c_d_", parsed.at(0).value);
}

TEST(parse_properties_test, should_allow_numbers_in_property_names_and_values)
{
    auto parsed = parse_properties("3f5a1: 2c5d2");
    ASSERT_EQ("3f5a1", parsed.at(0).name);
    ASSERT_EQ("2c5d2", parsed.at(0).value);
}

TEST(parse_properties_test, should_parse_values_in_single_quotes)
{
    auto parsed = parse_properties("something: \'a and b\'");
    ASSERT_EQ("something", parsed.at(0).name);
    ASSERT_EQ("a and b", parsed.at(0).value);
}


}
}
