#include "config/config.hpp"
#include "find_cpp_files.hpp"
#include <boost/filesystem.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/algorithm/string/join.hpp>

namespace cell
{
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
            build_command(file);
    }

    path get_file() const
    {
        return file;
    }

    void set_build_command(std::function<void(const path& out)> cmd)
    {
        build_command = std::move(cmd);
    }

    Target(path file) : file(std::move(file)) { }
private:
    path file;
    std::vector<Target> dependencies;
    std::function<void(const path& out)> build_command;

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

void build_targets(const configuration& configuration, const paths& cpps, std::function<void(const path& , const path& )> compile, std::function<void(const std::vector<path>& , const path& )> link)
{
    Target tgt{configuration.project_name};
    std::vector<path> ofiles;
    for (auto cpp : cpps)
    {
        auto ofile = replace_extension(cpp, ".o");
        ofiles.push_back(ofile);
        Target target{ofile};
        target.set_build_command([=](const path& out){ compile(cpp, out); });
        target.add_dependency(cpp);
        tgt.add_dependency(std::move(target));
    }
    tgt.set_build_command([=](const path& out){ link(ofiles, out); });
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
    std::function<std::string(const path& e)> to_string = [](const path& p) { return p.string(); };
    auto flattened = boost::join(transform(ofiles, to_string), " ");
    auto link_cmd = "g++ " + flattened + " -o " + target.string();
    std::system(link_cmd.c_str());
}

void build_targets(const configuration& configuration, const paths& cpps)
{
    build_targets(configuration, cpps, compile_cpp, link_target);
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
