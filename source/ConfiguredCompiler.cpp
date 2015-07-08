#include "ConfiguredCompiler.hpp"
#include <boost/range/adaptors.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace cell {

void ConfiguredCompiler::compile(const boost::filesystem::path& cpp, const boost::filesystem::path& ofile)
{
    auto args = desc.compile_source;
    boost::replace_all(args, "$(SOURCE)", cpp.string());
    boost::replace_all(args, "$(OBJECT)", ofile.string());
    auto compile_cmd = desc.executable + " " + args;
    std::system(compile_cmd.c_str());
}

void ConfiguredCompiler::link(const std::vector< path >& ofiles, const path& target)
{
    using namespace boost::adaptors;
    std::function<std::string(const path& e)> to_string = [](const path& p) { return p.string(); };
    auto flattened = boost::join(transform(ofiles, to_string), " ");
    auto args = desc.link_executable;
    boost::replace_all(args, "$(OBJECTS)", flattened);
    boost::replace_all(args, "$(EXECUTABLE)", target.string());
    auto link_cmd = desc.executable + " " + args;
    std::system(link_cmd.c_str());
}

}
