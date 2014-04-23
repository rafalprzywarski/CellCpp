#include "config/config.hpp"
#include <boost/filesystem.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/algorithm/string/join.hpp>

namespace cell
{

std::vector<boost::filesystem::path> find_cpp_files()
{
    using namespace boost::filesystem;
    using namespace boost::range;
    using namespace boost::adaptors;
    std::function<bool(const directory_entry& e)> is_cpp = [](const directory_entry& e) { return e.path().extension() == ".cpp"; };
    std::function<std::string(const directory_entry& e)> to_string = [](const directory_entry& e) { return e.path().string(); };
    decltype(find_cpp_files()) cpps;
    push_back(cpps, transform(filter(make_iterator_range(directory_iterator(current_path()), directory_iterator()), is_cpp), to_string));
    return cpps;
}

boost::filesystem::path replace_extension(boost::filesystem::path path, const boost::filesystem::path& new_ext)
{
    return std::move(path.replace_extension(new_ext));
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

    boost::filesystem::path get_file() const
    {
        return file;
    }

    void set_build_command(std::function<void(const boost::filesystem::path& out)> cmd)
    {
        build_command = std::move(cmd);
    }

    Target(boost::filesystem::path file) : file(std::move(file)) { }
private:
    boost::filesystem::path file;
    std::vector<Target> dependencies;
    std::function<void(const boost::filesystem::path& out)> build_command;

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

void build_targets(const configuration& configuration, std::function<void(const boost::filesystem::path& , const boost::filesystem::path& )> compile, std::function<void(const std::vector<boost::filesystem::path>& , const boost::filesystem::path& )> link)
{
    Target tgt{configuration.project_name};
    std::vector<boost::filesystem::path> ofiles;
    for (auto cpp : find_cpp_files())
    {
        auto ofile = replace_extension(cpp, ".o");
        ofiles.push_back(ofile);
        Target target{ofile};
        target.set_build_command([=](const boost::filesystem::path& out){ compile(cpp, out); });
        target.add_dependency(cpp);
        tgt.add_dependency(std::move(target));
    }
    tgt.set_build_command([=](const boost::filesystem::path& out){ link(ofiles, out); });
    tgt.build();
}

void compile_cpp(const boost::filesystem::path& cpp, const boost::filesystem::path& ofile)
{
    auto compile_cmd = "g++ -c " + cpp.string() + " -o " + ofile.string();
    std::system(compile_cmd.c_str());
}

void link_target(const std::vector<boost::filesystem::path>& ofiles, const boost::filesystem::path& target)
{
    using namespace boost::adaptors;
    std::function<std::string(const boost::filesystem::path& e)> to_string = [](const boost::filesystem::path& p) { return p.string(); };
    auto flattened = boost::join(transform(ofiles, to_string), " ");
    auto link_cmd = "g++ " + flattened + " -o " + target.string();
    std::system(link_cmd.c_str());
}

void build_targets(const configuration& configuration)
{
    build_targets(configuration, compile_cpp, link_target);
}

void run()
{
    auto configuration = load_configuration();
    build_targets(configuration);
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
