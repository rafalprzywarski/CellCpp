#include <gmock/gmock.h>
#include <config/config.hpp>
#include <config/property.hpp>
#include <boost/filesystem/path.hpp>
#include <functional>

namespace cell
{

struct load_configuration_test : testing::Test
{
    std::map<path, config::properties> content;

    config::properties load_file(const path& filename)
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

TEST_F(load_configuration_test, should_parse_project_name)
{
    content["build.cell"] = { { "project", "some123" }};
    auto configuration = load_configuration();
    EXPECT_EQ("some123", configuration.project_name);
}

TEST_F(load_configuration_test, should_parse_executable_name)
{
    content["build.cell"] = { { "executable", "myexe" }};
    auto configuration = load_configuration();
    EXPECT_EQ("myexe", configuration.executable_name);
}

TEST_F(load_configuration_test, should_use_project_name_if_no_exectable_name_is_specified)
{
    content["build.cell"] = { { "project", "123" } };
    EXPECT_EQ("123", load_configuration().executable_name);
}

TEST_F(load_configuration_test, should_return_gcc_as_the_default_compiler)
{
    content["build.cell"] = { { "project", "123" } };
    auto compiler = load_configuration().compiler;
    // TODO: a temporary hack
    EXPECT_EQ("g++ -c $(SOURCE) -o $(OBJECT)", compiler.compile_source);
    EXPECT_EQ("g++ $(OBJECTS) -o $(EXECUTABLE)", compiler.link_executable);
    EXPECT_EQ("g++ -MM $(SOURCE)", compiler.get_used_headers);
}

TEST_F(load_configuration_test, should_load_compiler_configuration)
{
    content["build.cell"] = { { "project", "123" }, { "compiler", "pretty" } };
    content["pretty.cell"] = {
        { "compile-source", "./path/prettycc cc -c" }, { "link-executable", "./path/prettycc ll -l" }, { "get-used-headers", "./path/prettycc hh -h" } };
    auto compiler = load_configuration().compiler;
    EXPECT_EQ("./path/prettycc cc -c", compiler.compile_source);
    EXPECT_EQ("./path/prettycc ll -l", compiler.link_executable);
    EXPECT_EQ("./path/prettycc hh -h", compiler.get_used_headers);
}

TEST_F(load_configuration_test, should_return_default_output_path)
{
    content["build.cell"] = { { "project", "123" } };
    EXPECT_EQ("build", load_configuration().output_path);
}

TEST_F(load_configuration_test, should_load_default_output_path)
{
    content["build.cell"] = { { "project", "123" }, { "output-path", "different/path" } };
    EXPECT_EQ("different/path", load_configuration().output_path);
}

}
