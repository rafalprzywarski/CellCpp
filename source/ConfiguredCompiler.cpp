#include "ConfiguredCompiler.hpp"
#include <boost/range/adaptors.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <cstdlib>

namespace cell {

namespace
{

std::string replace_properties(std::string s, const config::properties& props)
{
    for (auto& p : props)
        boost::replace_all(s, "$(" + p.name + ')', p.value);
    return s;
}

std::string make_command(const std::string& executable, const std::string& args, const config::properties& props)
{
    return executable + " " + replace_properties(args, props);
}

void execute_command(const std::string& executable, const std::string& args, const config::properties& props)
{
    std::system(make_command(executable, args, props).c_str());
}

}

void ConfiguredCompiler::compile(const boost::filesystem::path& cpp, const boost::filesystem::path& ofile)
{
    config::properties props = {{ "SOURCE", cpp.string() }, { "OBJECT", ofile.string() }};
    execute_command(desc.executable, desc.compile_source, props);
}

void ConfiguredCompiler::link(const std::vector< path >& ofiles, const path& target)
{
    using namespace boost::adaptors;
    std::function<std::string(const path& e)> to_string = [](const path& p) { return p.string(); };
    auto flattened = boost::join(transform(ofiles, to_string), " ");
    config::properties props = {{ "OBJECTS", flattened }, { "EXECUTABLE", target.string() }};
    execute_command(desc.executable, desc.link_executable, props);
}

std::string ConfiguredCompiler::get_command_output(const std::string& cmd)
{
  auto file = popen(cmd.c_str(), "r");
  char buffer[1024];
  std::fgets(buffer, sizeof(buffer), file);
  pclose(file);
  return buffer;
}

paths ConfiguredCompiler::get_required_headers(const path& cppfile)
{
    if (desc.get_used_headers.empty())
        return {};
    config::properties props = {{ "SOURCE", cppfile.string() }};
    std::string text = get_command_output(make_command(desc.executable, desc.get_used_headers, props));
    std::vector<std::string> files;
    boost::split(files, text, boost::is_any_of(" "), boost::token_compress_on);
    return {files.begin() + 2, files.end()};
}

}
