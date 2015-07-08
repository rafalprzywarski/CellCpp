#include "config/config.hpp"
#include "find_cpp_files.hpp"
#include <boost/filesystem.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <functional>
#include <iostream>

namespace cell
{

template <typename F>
using fn = std::function<F>;

path replace_extension(path p, const path& new_ext)
{
    return std::move(p.replace_extension(new_ext));
}

struct Target
{
    void add_dependency(Target dep)
    {
        dependencies.push_back(std::move(dep));
    }

    void set_dependencies(std::vector<Target> deps)
    {
        dependencies = std::move(deps);
    }

    void build()
    {
        for (auto& dep : dependencies)
            dep.build();
        if (is_out_of_date())
            build_command();
    }

    void set_build_command(fn<void()> cmd)
    {
        build_command = std::move(cmd);
    }

    Target(path file) : file(std::move(file)) { }
private:
    path file;
    std::vector<Target> dependencies;
    fn<void()> build_command;

    bool is_out_of_date() const
    {
        if (!exists(file))
            return true;
        auto modification_time = get_modification_time();
        return boost::find_if(dependencies, [=](const Target& dep) { return dep.get_modification_time() > modification_time; }) != dependencies.end();
    }

    std::time_t get_modification_time() const
    {
        return last_write_time(file);
    }
};

Target create_object_target(const path& cpp, const path& obj, fn<void(const path& , const path& )> compile)
{
    Target target{obj};
    target.set_build_command([=](){ compile(cpp, obj); });
    target.add_dependency(cpp);
    return target;
}

Target create_executable_target(const paths& objs, const path& exe, std::vector<Target> deps, fn<void(const paths& , const path& )> link)
{
    Target target{exe};
    target.set_build_command([=](){ link(objs, exe); });
    target.set_dependencies(std::move(deps));
    return target;
}

void build_targets(const std::string& executable_name, const paths& cpps, fn<Target(const path& , const path& )> create_object_target, fn<void(const std::vector<path>& , const path& )> link)
{
    std::vector<path> ofiles;
    std::vector<Target> deps;
    for (auto cpp : cpps)
    {
        auto ofile = replace_extension(cpp, ".o");
        ofiles.push_back(ofile);
        deps.push_back(create_object_target(cpp, ofile));
    }
    auto tgt = create_executable_target(ofiles, executable_name, deps, link);
    tgt.build();
}

void compile_cpp(const compiler_desc& compiler, const path& cpp, const path& ofile)
{
    auto args = compiler.compile_source;
    boost::replace_all(args, "$(SOURCE)", cpp.string());
    boost::replace_all(args, "$(OBJECT)", ofile.string());
    auto compile_cmd = compiler.executable + " " + args;
    std::system(compile_cmd.c_str());
}

void link_target(const compiler_desc& compiler, const std::vector<path>& ofiles, const path& target)
{
    using namespace boost::adaptors;
    fn<std::string(const path& e)> to_string = [](const path& p) { return p.string(); };
    auto flattened = boost::join(transform(ofiles, to_string), " ");
    auto args = compiler.link_executable;
    boost::replace_all(args, "$(OBJECTS)", flattened);
    boost::replace_all(args, "$(EXECUTABLE)", target.string());
    auto link_cmd = compiler.executable + " " + args;
    std::system(link_cmd.c_str());
}

void build_targets(const configuration& configuration, const paths& cpps)
{
    using namespace std::placeholders;
    using std::bind;
    fn<void(const path& , const path& )> compile = bind(compile_cpp, configuration.compiler, _1, _2);
    build_targets(
        configuration.executable_name,
        cpps,
        bind(create_object_target, _1, _2, compile),
        bind(link_target, configuration.compiler, _1, _2));
}

void run()
{
    auto configuration = load_configuration();
    auto cpps = find_cpp_files();
    build_targets(configuration, cpps);
}

}

int main()
{
    try
    {
        cell::run();
    }
    catch (const cell::configuration_error& error)
    {
        std::cerr << error.what() << std::endl;
        return 1;
    }
    return 0;
}
