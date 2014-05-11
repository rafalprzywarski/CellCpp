#include <gmock/gmock.h>
#include <config/config.hpp>
#include <boost/filesystem/path.hpp>
#include <functional>

namespace cell
{

struct load_configuration_test : testing::Test
{
    std::map<path, std::string> content;

    std::string load_file(const path& filename)
    {
        auto it = content.find(filename);
        if (it == content.end())
            throw configuration_error("File " + filename.string() + " not found");
        return it->second;
    }

    configuration load_configuration()
    {
        return cell::load_configuration(std::bind(&load_configuration_test::load_file, this, std::placeholders::_1));
    }
};

struct content_with_project_name
{
    std::string content, project_name;
};

std::ostream& operator<<(std::ostream& os, const content_with_project_name& p)
{
    return os << p.content;
}

struct parse_project_name_test : load_configuration_test, testing::WithParamInterface<content_with_project_name> { };

TEST_P(parse_project_name_test, should_parse_project_name)
{
    content["build.cell"] = GetParam().content;
    auto configuration = load_configuration();
    EXPECT_EQ(GetParam().project_name, configuration.project_name);
}

struct content_with_executable_name
{
    std::string content, executable_name;
};

std::ostream& operator<<(std::ostream& os, const content_with_executable_name& p)
{
    return os << p.content;
}

struct parse_executable_name_test : load_configuration_test, testing::WithParamInterface<content_with_executable_name> { };

TEST_P(parse_executable_name_test, should_parse_executable_name)
{
    content["build.cell"] = GetParam().content;
    auto configuration = load_configuration();
    EXPECT_EQ(GetParam().executable_name, configuration.executable_name);
}

TEST_F(parse_executable_name_test, should_use_project_name_if_no_exectable_name_is_specified)
{
    content["build.cell"] = "project: 123";
    EXPECT_EQ("123", load_configuration().executable_name);
}

TEST_F(load_configuration_test, should_return_gcc_as_the_default_compiler)
{
    content["build.cell"] = "project: 123";
    EXPECT_EQ("g++", load_configuration().compiler.executable); // TODO: a temporary hack
}

TEST_F(load_configuration_test, should_load_compiler_configuration)
{
    content["build.cell"] = "project: 123\ncompiler: pretty";
    content["pretty.cell"] = "executable: \'./path/prettycc\'\ncompile-source: \'cc -c\'\nlink-executable: \'ll -l\'";
    auto compiler = load_configuration().compiler;
    EXPECT_EQ("./path/prettycc", compiler.executable);
    EXPECT_EQ("cc -c", compiler.compile_source);
    EXPECT_EQ("ll -l", compiler.link_executable);
}

INSTANTIATE_TEST_CASE_P(all_cases, parse_project_name_test, testing::Values(
    content_with_project_name{"project: abc123", "abc123"},
    content_with_project_name{"project :other", "other"},
    content_with_project_name{"project : another\n", "another"}));

INSTANTIATE_TEST_CASE_P(all_cases, parse_executable_name_test, testing::Values(
    content_with_executable_name{"project: X\nexecutable: xyz345", "xyz345"},
    content_with_executable_name{"executable : other\nproject: X", "other"}));

}
