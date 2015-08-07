#ifndef CELL_DAC370A2C54A402F99E7114DAF1A4522_HPP
#define CELL_DAC370A2C54A402F99E7114DAF1A4522_HPP
#include <boost/optional.hpp>
#include <iterator>
#include <fstream>
#include "fn.hpp"

namespace cell
{

struct Target
{
    void build()
    {
        for (auto& dep : dependencies)
            dep.build();
        if (is_out_of_date())
            build_command();
    }

    static boost::optional<Target> load(path cpp, path obj, fn<void()> build_command)
    {
        std::ifstream f(cpp.string() + ".d");
        if (!f.is_open())
          return {};
        std::vector<Target> dependencies{std::istream_iterator<path>(f), std::istream_iterator<path>()};
        return Target{obj, dependencies, build_command};
    }

    void store(path cpp)
    {
        std::ofstream f(cpp.string() + ".d");
        for (auto& dep : dependencies)
          f << dep.file << '\n';
    }

    Target(path file) : file(std::move(file)) { }

    Target(path file, std::vector<Target> dependencies, fn<void()> build_command) : file(std::move(file)), dependencies(std::move(dependencies)), build_command(build_command) { }
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

}
#endif
