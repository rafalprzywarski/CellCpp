#include "config/config.hpp"
#include "find_cpp_files.hpp"
#include <boost/filesystem.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/algorithm/string/join.hpp>
#include <functional>

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

void build_targets(const configuration& configuration, const paths& cpps, fn<Target(const path& , const path& )> create_object_target, fn<void(const std::vector<path>& , const path& )> link)
{
    Target tgt{configuration.executable_name};
    std::vector<path> ofiles;
    for (auto cpp : cpps)
    {
        auto ofile = replace_extension(cpp, ".o");
        ofiles.push_back(ofile);
        tgt.add_dependency(create_object_target(cpp, ofile));
    }
    tgt.set_build_command([=](){ link(ofiles, configuration.executable_name); });
    tgt.build();
}

void compile_cpp(const path& cpp, const path& ofile)
{
    auto compile_cmd = "g++ -c " + cpp.string() + " -o " + ofile.string();
    std::system(compile_cmd.c_str());
}

void link_target(const std::vector<path>& ofiles, const path& target)
{
    using namespace boost::adaptors;
    fn<std::string(const path& e)> to_string = [](const path& p) { return p.string(); };
    auto flattened = boost::join(transform(ofiles, to_string), " ");
    auto link_cmd = "g++ " + flattened + " -o " + target.string();
    std::system(link_cmd.c_str());
}

void build_targets(const configuration& configuration, const paths& cpps)
{
    using namespace std::placeholders;
    build_targets(configuration, cpps, std::bind(create_object_target, _1, _2, compile_cpp), link_target);
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
