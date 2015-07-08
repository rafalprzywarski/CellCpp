#include "find_cpp_files.hpp"
#include <boost/range/iterator_range.hpp>
#include <boost/filesystem.hpp>

namespace cell
{

paths find_cpp_files()
{
    using namespace boost::filesystem;
    decltype(find_cpp_files()) cpps;
    for (auto& entry : boost::make_iterator_range(directory_iterator(current_path()), directory_iterator()))
        if (entry.path().extension() == ".cpp")
            cpps.push_back(entry.path().filename().string());
    return cpps;
}

}
