#include <gmock/gmock.h>
#include <config/config.hpp>
#include <boost/filesystem/path.hpp>
#include <functional>

namespace cell
{

struct config_test : testing::Test
{
    std::map<boost::filesystem::path, std::string> content;

    std::string load_file(const boost::filesystem::path& filename)
    {
        auto it = content.find(filename);
        if (it == content.end())
            throw configuration_error("File " + filename.string() + " not found");
        return it->second;
    }

    configuration load_configuration()
    {
        return cell::load_configuration(std::bind(&config_test::load_file, this, std::placeholders::_1));
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

struct parse_project_name_test : config_test, testing::WithParamInterface<content_with_project_name> { };

TEST_P(parse_project_name_test, should_parse_project_name)
{
    content["build.cell"] = GetParam().content;
    auto configuration = load_configuration();
    EXPECT_EQ(GetParam().project_name, configuration.project_name);
}

INSTANTIATE_TEST_CASE_P(all_cases, parse_project_name_test, testing::Values(
    content_with_project_name{"project: abc123", "abc123"},
    content_with_project_name{"project :other", "other"},
    content_with_project_name{"project : another\n", "another"}));

}
